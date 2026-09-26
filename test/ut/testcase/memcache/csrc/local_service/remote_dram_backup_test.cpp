/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * MemCache_Hybrid is licensed under Mulan PSL v2.
 */
#include <cstdlib>
#include "gtest/gtest.h"
#include "mmc_local_service_default.h"

namespace ock {
namespace mmc {
namespace {
using BackupMap = std::map<std::string, std::vector<MmcMemBlobDesc>>;
struct RemoteBackupMapMember {
    using Type = BackupMap MmcLocalServiceDefault::*;
    friend Type RemoteMap(RemoteBackupMapMember);
};
template<typename Tag, typename Tag::Type member>
struct RemoteBackupAccessor {
    friend typename Tag::Type RemoteMap(Tag) { return member; }
};
template struct RemoteBackupAccessor<RemoteBackupMapMember, &MmcLocalServiceDefault::blobMap_>;

class RemoteDramBackupTest : public testing::Test {
protected:
    void SetUp() override
    {
        const char *old = std::getenv("UBSIO_KVC_MODE");
        hadMode_ = old != nullptr;
        if (hadMode_) oldMode_ = old;
        setenv("UBSIO_KVC_MODE", "remote_dram_tcp", 1);
    }
    void TearDown() override
    {
        if (hadMode_) setenv("UBSIO_KVC_MODE", oldMode_.c_str(), 1);
        else unsetenv("UBSIO_KVC_MODE");
    }
    MmcLocalServiceDefault local_{"remote-dram-backup-test"};
    std::string oldMode_;
    bool hadMode_ = false;
};
} // namespace

TEST_F(RemoteDramBackupTest, OldRemoveDoesNotEraseRecreatedLower)
{
    constexpr uint64_t oldGeneration = 101;
    constexpr uint64_t newGeneration = 102;
    constexpr uint64_t size = 4096;
    constexpr uint32_t rank = 3;
    const MmcMemBlobDesc old{rank, oldGeneration, size, MEDIA_SSD};
    const MmcMemBlobDesc current{rank, newGeneration, size, MEDIA_SSD};
    auto &map = local_.*RemoteMap(RemoteBackupMapMember{});
    map["same-key"] = {current};
    std::vector<Result> results;
    EXPECT_EQ(local_.UpdateMetaBackup({META_BACKUP_REMOVE}, {"same-key"}, {old}, results, {0}), MMC_OK);
    EXPECT_EQ(results, std::vector<Result>{MMC_UNMATCHED_KEY});
    ASSERT_EQ(map["same-key"].size(), 1U);
    EXPECT_EQ(map["same-key"].front(), current);
    EXPECT_EQ(local_.UpdateMetaBackup({META_BACKUP_REMOVE}, {"same-key"}, {current}, results, {0}), MMC_OK);
    EXPECT_TRUE(map.empty());
}

TEST_F(RemoteDramBackupTest, MissingOrZeroGenerationFailsBeforeRecoveryMutation)
{
    constexpr uint64_t gva = 4096;
    constexpr uint32_t rank = 3;
    const MmcMemBlobDesc dram{rank, gva, gva, MEDIA_DRAM};
    std::vector<Result> results;
    EXPECT_EQ(local_.UpdateMetaBackup({META_BACKUP_ADD}, {"key"}, {dram}, results), MMC_INVALID_PARAM);
    EXPECT_EQ(local_.UpdateMetaBackup({META_BACKUP_ADD}, {"key"}, {dram}, results, {0}), MMC_INVALID_PARAM);
    EXPECT_TRUE((local_.*RemoteMap(RemoteBackupMapMember{})).empty());
}
} // namespace mmc
} // namespace ock
