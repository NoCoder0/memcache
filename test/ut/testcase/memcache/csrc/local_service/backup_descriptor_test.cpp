/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * MemCache_Hybrid is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
#include "gtest/gtest.h"
#include "mmc_local_service_default.h"

namespace ock {
namespace mmc {
namespace {
using BackupMap = std::map<std::string, std::vector<MmcMemBlobDesc>>;
struct BackupMapMember {
    using Type = BackupMap MmcLocalServiceDefault::*;
    friend Type BackupMapAccessor(BackupMapMember);
};
template<typename Tag, typename Tag::Type member>
struct BackupAccessor {
    friend typename Tag::Type BackupMapAccessor(Tag)
    {
        return member;
    }
};
template struct BackupAccessor<BackupMapMember, &MmcLocalServiceDefault::blobMap_>;
struct MetaEventMember {
    using Type = void (MmcLocalServiceDefault::*)(int, const std::vector<std::string> &);
    friend Type BackupMapAccessor(MetaEventMember);
};
template struct BackupAccessor<MetaEventMember, &MmcLocalServiceDefault::HandleUbsIoMetaEvents>;
struct LocalOptionsMember {
    using Type = mmc_local_service_config_t MmcLocalServiceDefault::*;
    friend Type BackupMapAccessor(LocalOptionsMember);
};
template struct BackupAccessor<LocalOptionsMember, &MmcLocalServiceDefault::options_>;
} // namespace

TEST(BackupDescriptorTest, RemoveMatchesTheCompleteDescriptor)
{
    constexpr uint64_t size = 4096;
    constexpr uint32_t rank = 3;
    constexpr uint32_t otherRank = 4;
    const MmcMemBlobDesc lower{rank, 0, size, MEDIA_SSD};
    const MmcMemBlobDesc other{otherRank, 0, size, MEDIA_SSD};
    MmcLocalServiceDefault local{"backup-descriptor-test"};
    auto &map = local.*BackupMapAccessor(BackupMapMember{});
    map["same-key"] = {lower};
    std::vector<Result> results;
    EXPECT_EQ(local.UpdateMetaBackup({META_BACKUP_REMOVE}, {"same-key"}, {other}, results), MMC_OK);
    EXPECT_EQ(results, std::vector<Result>{MMC_UNMATCHED_KEY});
    EXPECT_EQ(map.at("same-key"), std::vector<MmcMemBlobDesc>{lower});
    EXPECT_EQ(local.UpdateMetaBackup({META_BACKUP_REMOVE}, {"same-key"}, {lower}, results), MMC_OK);
    EXPECT_EQ(results, std::vector<Result>{MMC_OK});
    EXPECT_TRUE(map.empty());

    EXPECT_EQ(local.UpdateMetaBackup({META_BACKUP_REMOVE}, {"same-key"}, {lower}, results), MMC_OK);
    EXPECT_EQ(results, std::vector<Result>{MMC_UNMATCHED_KEY});
}

TEST(BackupDescriptorTest, MetaEventsFollowMemCacheStorageMode)
{
    constexpr uint64_t blobSize = 4096;
    MmcLocalServiceDefault local{"storage-mode-test"};
    auto &options = local.*BackupMapAccessor(LocalOptionsMember{});
    auto &map = local.*BackupMapAccessor(BackupMapMember{});
    const MmcMemBlobDesc lower{options.rankId, 0, blobSize, MEDIA_SSD};
    map["same-key"] = {lower};

    options.storageRemoteDram = true;
    (local.*BackupMapAccessor(MetaEventMember{}))(UBSIO_META_DELETE, {"same-key"});
    EXPECT_EQ(map.at("same-key"), std::vector<MmcMemBlobDesc>{lower});

    options.storageRemoteDram = false;
    (local.*BackupMapAccessor(MetaEventMember{}))(UBSIO_META_DELETE, {"same-key"});
    EXPECT_TRUE(map.empty());
}

} // namespace mmc
} // namespace ock
