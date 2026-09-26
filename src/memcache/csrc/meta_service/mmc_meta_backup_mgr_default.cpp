/*
* Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * MemCache_Hybrid is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
*/

#include "mmc_meta_backup_mgr_default.h"

#include "mmc_meta_backup_mgr_factory.h"
#include "mmc_mem_obj_meta.h"
#include "mmc_msg_client_meta.h"
#include "mmc_ptracer.h"
#include "mmc_remote_dram.h"

namespace ock {
namespace mmc {

constexpr int BACKUP_RPC_TIMEOUT_SECOND = 60;

std::map<std::string, MmcRef<MMCMetaBackUpMgr>> MMCMetaBackUpMgrFactory::instances_;
std::mutex MMCMetaBackUpMgrFactory::instanceMutex_;

void MMCMetaBackUpMgrDefault::BackupThreadFunc()
{
    auto interval = std::chrono::milliseconds(asyncFlushIntervalMs_);
    while (true) {
        {
            std::unique_lock<std::mutex> lock(backupThreadLock_);
            backupThreadCv_.wait_for(lock, interval, [this] { return !backupList_.empty() || !started_; });
        }
        if (!started_) {
            MMC_LOG_TRACE("backup thread destroy, thread id " << pthread_self());
            break;
        }

        SendBackup2Local();
    }
}

void MMCMetaBackUpMgrDefault::SendBackup2Local()
{
    if (metaNetServer_ == nullptr) {
        MMC_LOG_WARN("MMCMetaBackUpMgr back up net not start");
        return;
    }

    // 锁内 splice 移出全部（O(1)），锁外按 rank 分组
    std::map<uint32_t, std::vector<MetaBackUpOperate>> rankGroups;
    std::list<MetaBackUpOperate> drained;
    {
        std::lock_guard<std::mutex> lg(backupThreadLock_);
        drained.splice(drained.begin(), backupList_);
    }
    const size_t batchLimit = asyncFlushBatchLimit_;
    for (auto it = drained.begin(); it != drained.end();) {
        auto &group = rankGroups[it->desc_.rank_];
        if (group.size() < batchLimit) {
            group.push_back(std::move(*it));
            it = drained.erase(it);
        } else {
            ++it;
        }
    }
    if (!drained.empty()) {
        std::lock_guard<std::mutex> lg(backupThreadLock_);
        backupList_.splice(backupList_.begin(), drained);
    }

    if (rankGroups.empty()) {
        return;
    }

    MMC_LOG_DEBUG("Backup sending " << rankGroups.size() << " rank groups");
    if (backupPool_ != nullptr) {
        std::vector<std::future<void>> futures;
        for (auto &[rank, entries] : rankGroups) {
            auto future = backupPool_->Enqueue([this, rank, &entries]() { SendBackupForRank(rank, entries); });
            if (!future.valid()) {
                // 线程池不可用（如正在停止），原地处理，保证备份/刷盘请求不丢
                MMC_LOG_WARN("backup pool unavailable, send backup for rank=" << rank << " inline");
                SendBackupForRank(rank, entries);
                continue;
            }
            futures.push_back(std::move(future));
        }
        for (auto &f : futures) {
            f.wait();
        }
    } else {
        for (auto &[rank, entries] : rankGroups) {
            SendBackupForRank(rank, entries);
        }
    }
}

void MMCMetaBackUpMgrDefault::SendBackupForRank(uint32_t rank, std::vector<MetaBackUpOperate> &entries)
{
    MetaReplicateRequest request;
    request.msgVer = RemoteDramEnabled() ? 1 : 0;
    std::map<size_t, BackupReadLease> leases;
    struct LeaseScope {
        std::map<size_t, BackupReadLease> &held;
        std::function<void(const BackupReadLease &)> &release;
        ~LeaseScope()
        {
            if (release) {
                for (const auto &item : held) release(item.second);
            }
        }
    } leaseScope{leases, releaseReadLease_};
    request.ops_.reserve(entries.size());
    request.keys_.reserve(entries.size());
    request.blobs_.reserve(entries.size());
    for (auto &e : entries) {
        if (e.op_ == META_BACKUP_ADD && e.desc_.mediaType_ == MEDIA_DRAM) {
            BackupReadLease lease;
            if (!acquireReadLease_ || !releaseReadLease_ || acquireReadLease_(e.key_, e.desc_, lease) != MMC_OK) {
                MMC_LOG_DEBUG("Skip backup without a DRAM read lease, key=" << e.key_ << ", blob=" << e.desc_);
                continue;
            }
            leases.emplace(request.ops_.size(), std::move(lease));
        }
        request.ops_.push_back(e.op_);
        request.keys_.push_back(std::move(e.key_));
        request.blobs_.push_back(e.desc_);
        if (request.msgVer == 1) {
            request.lowerGenerations_.push_back(e.op_ == META_BACKUP_ADD && e.desc_.mediaType_ == MEDIA_DRAM ?
                                                  NewLowerGeneration() : 0);
        }
    }

    if (request.ops_.empty()) {
        return;
    }
    MMC_LOG_DEBUG("Backup flush rank=" << rank << " keys=" << request.keys_.size());

    Response response;
    TP_TRACE_BEGIN(TP_MMC_META_ASYNC_FLUSH_RPC);
    Result ret = metaNetServer_->SyncCall(rank, request, response, BACKUP_RPC_TIMEOUT_SECOND);
    TP_TRACE_END(TP_MMC_META_ASYNC_FLUSH_RPC, ret);
    if (ret != MMC_OK || response.ret_ != MMC_OK || response.msgVer != request.msgVer) {
        MMC_LOG_ERROR("mmc meta back up failed, bm rank " << rank << ", ret=" << ret
                                                          << ", keys: " << request.KeysString());
        return;
    }

    if (!onAsyncFlushComplete_) {
        return;
    }

    if (response.keyResults_.size() != request.ops_.size()) {
        MMC_LOG_ERROR("keyResults size mismatch, response=" << response.keyResults_.size()
                                                            << ", request=" << request.ops_.size());
        return;
    }

    std::vector<AsyncFlushBlob> flushedBlobs;
    for (size_t i = 0; i < response.keyResults_.size(); ++i) {
        if (request.ops_[i] == META_BACKUP_ADD) {
            if (isSsdAvailableFunc_ == nullptr || !isSsdAvailableFunc_(rank)) {
                // 非刷盘模式：ADD 成功仅表示元数据已记录，不构造 SSD 副本
                if (response.keyResults_[i] != MMC_OK) {
                    MMC_LOG_WARN("backup add failed for key=" << request.keys_[i]
                                                              << ", ret=" << response.keyResults_[i]);
                }
                continue;
            }
            if (response.keyResults_[i] == MMC_OK) {
                const auto lease = leases.find(i);
                if (lease == leases.end()) continue;
                MmcMemBlobDesc ssdDesc = request.blobs_[i];
                ssdDesc.mediaType_ = MEDIA_SSD;
                ssdDesc.gva_ = request.msgVer == 1 ? request.lowerGenerations_[i] : 0;
                flushedBlobs.push_back({request.keys_[i], request.blobs_[i], ssdDesc, lease->second});
            } else {
                MMC_LOG_WARN("async flush failed for key=" << request.keys_[i] << ", ret=" << response.keyResults_[i]);
            }
        } else if (response.keyResults_[i] != MMC_OK) {
            MMC_LOG_WARN("backup op failed, op=" << request.ops_[i] << ", key=" << request.keys_[i]
                                                 << ", ret=" << response.keyResults_[i]);
        }
    }
    if (!flushedBlobs.empty()) {
        onAsyncFlushComplete_(rank, flushedBlobs);
    }
}

} // namespace mmc
} // namespace ock
