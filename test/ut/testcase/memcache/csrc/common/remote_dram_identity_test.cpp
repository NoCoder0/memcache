/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * MemCache_Hybrid is licensed under Mulan PSL v2.
 */
#include <cassert>
#include <map>
#include <set>
#include <tuple>
#include "mmc_remote_dram.h"

#ifdef MMC_REMOTE_DRAM_STANDALONE_TEST
// Portable harness for the production template algorithms; full UT uses the real descriptor below.
struct TestDesc {
    uint32_t rank_;
    uint64_t gva_;
    uint64_t size_;
    uint16_t mediaType_;
    bool operator==(const TestDesc &other) const
    {
        return std::tie(rank_, gva_, size_, mediaType_) ==
               std::tie(other.rank_, other.gva_, other.size_, other.mediaType_);
    }
};
constexpr uint16_t TEST_DRAM = 1;
constexpr uint16_t TEST_LOWER = 2;
#define TEST(suite, name) void name()
#define EXPECT_EQ(a, b) assert((a) == (b))
#define EXPECT_NE(a, b) assert((a) != (b))
#define EXPECT_TRUE(a) assert(a)
#define EXPECT_FALSE(a) assert(!(a))
#else
#include "gtest/gtest.h"
#include "mmc_msg_client_meta.h"
using TestDesc = ock::mmc::MmcMemBlobDesc;
constexpr uint16_t TEST_DRAM = ock::mmc::MEDIA_DRAM;
constexpr uint16_t TEST_LOWER = ock::mmc::MEDIA_SSD;
#endif

using namespace ock::mmc;
namespace {
constexpr uint32_t TEST_RANK = 3;
constexpr uint64_t TEST_BYTES = 4096;
constexpr uint64_t OLD_GENERATION = 101;
constexpr uint64_t NEW_GENERATION = 102;

struct RemoteModeScope {
    std::string previous;
    bool hadPrevious;
    RemoteModeScope()
    {
        const char *old = std::getenv("UBSIO_KVC_MODE");
        hadPrevious = old != nullptr;
        if (old != nullptr) previous = old;
#ifdef _WIN32
        _putenv_s("UBSIO_KVC_MODE", "remote_dram_tcp");
#else
        setenv("UBSIO_KVC_MODE", "remote_dram_tcp", 1);
#endif
    }
    ~RemoteModeScope()
    {
#ifdef _WIN32
        _putenv_s("UBSIO_KVC_MODE", previous.c_str());
#else
        if (hadPrevious) setenv("UBSIO_KVC_MODE", previous.c_str(), 1);
        else unsetenv("UBSIO_KVC_MODE");
#endif
    }
};
} // namespace

TEST(RemoteDramIdentity, DelayedRemovalPreservesSuccessor)
{
    const TestDesc old{TEST_RANK, OLD_GENERATION, TEST_BYTES, TEST_LOWER};
    const TestDesc current{TEST_RANK, NEW_GENERATION, TEST_BYTES, TEST_LOWER};
    std::map<std::string, std::vector<TestDesc>> map{{"key", {current}}};
    EXPECT_FALSE(EraseExactBackup(map, "key", old));
    EXPECT_EQ(map.at("key").front(), current);
    EXPECT_TRUE(EraseExactBackup(map, "key", current));
    EXPECT_TRUE(map.empty());
}

TEST(RemoteDramIdentity, RejoinDropsOnlyUncommittedRemoteRecovery)
{
    const TestDesc dram{TEST_RANK, TEST_BYTES, TEST_BYTES, TEST_DRAM};
    const TestDesc lower{TEST_RANK, OLD_GENERATION, TEST_BYTES, TEST_LOWER};
    std::vector<TestDesc> descs{dram, lower};
    DropRemoteRecoveryDescriptors(descs, TEST_LOWER);
    EXPECT_EQ(descs.size(), 1U);
    EXPECT_EQ(descs.front(), dram);
    DropRemoteRecoveryDescriptors(descs, TEST_LOWER);
    EXPECT_EQ(descs.size(), 1U);
}

TEST(RemoteDramIdentity, PhysicalAddressesDoNotAliasAcrossRebuilds)
{
    RemoteModeScope mode;
    const TestDesc old{TEST_RANK, OLD_GENERATION, TEST_BYTES, TEST_LOWER};
    const TestDesc current{TEST_RANK, NEW_GENERATION, TEST_BYTES, TEST_LOWER};
    EXPECT_NE(LowerStorageKey("key", old), LowerStorageKey("key", current));
    EXPECT_NE(LowerStorageKey("key", old), LowerStorageKey("other", old));
    EXPECT_TRUE(LowerStorageKey(std::string(1024, 'x'), old).size() < 128U);
    std::set<uint64_t> generations;
    constexpr size_t ATTEMPTS = 10000;
    for (size_t i = 0; i < ATTEMPTS; ++i) generations.insert(NewLowerGeneration());
    EXPECT_EQ(generations.size(), ATTEMPTS);
    EXPECT_EQ(generations.count(0), 0U);
}

#ifndef MMC_REMOTE_DRAM_STANDALONE_TEST
TEST(RemoteDramIdentity, VersionedReplicationValidatesGenerationCount)
{
    MetaReplicateRequest request;
    request.msgVer = 1;
    request.ops_ = {META_BACKUP_ADD};
    request.keys_ = {"key"};
    request.blobs_ = {{TEST_RANK, TEST_BYTES, TEST_BYTES, TEST_DRAM}};
    request.lowerGenerations_ = {NEW_GENERATION};
    NetMsgPacker packer;
    EXPECT_EQ(request.Serialize(packer), MMC_OK);
    NetMsgUnpacker unpacker(packer.String());
    MetaReplicateRequest decoded;
    EXPECT_EQ(decoded.Deserialize(unpacker), MMC_OK);
    EXPECT_EQ(decoded.lowerGenerations_, request.lowerGenerations_);
    request.lowerGenerations_.clear();
    NetMsgPacker invalid;
    request.Serialize(invalid);
    NetMsgUnpacker invalidInput(invalid.String());
    EXPECT_EQ(decoded.Deserialize(invalidInput), MMC_INVALID_PARAM);
}
#else
int main()
{
    DelayedRemovalPreservesSuccessor();
    RejoinDropsOnlyUncommittedRemoteRecovery();
    PhysicalAddressesDoNotAliasAcrossRebuilds();
}
#endif
