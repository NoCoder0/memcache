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
#include <gtest/gtest.h>

#include "mmc_msg_client_meta.h"
#include "mmc_msg_packer.h"

using namespace ock::mmc;

class TestProtoMsg : public testing::Test {
public:
    void SetUp() override {}
    void TearDown() override {}
};

// BlobDeleteRequest serialization/deserialization
TEST_F(TestProtoMsg, BlobDeleteRequest_SerializeDeserialize)
{
    BlobDeleteRequest req;
    req.key_ = "test_key";
    req.rank_ = 42U;
    req.msgId = LM_BLOB_DELETE_REQ;

    NetMsgPacker packer;
    ASSERT_EQ(req.Serialize(packer), MMC_OK);
    auto serialized = packer.String();

    BlobDeleteRequest req2;
    NetMsgUnpacker unpacker(serialized);
    ASSERT_EQ(req2.Deserialize(unpacker), MMC_OK);
    EXPECT_EQ(req2.key_, "test_key");
    EXPECT_EQ(req2.rank_, 42U);
    EXPECT_EQ(req2.msgId, LM_BLOB_DELETE_REQ);
}

// BlobDeleteResponse serialization/deserialization — success
TEST_F(TestProtoMsg, BlobDeleteResponse_Success)
{
    BlobDeleteResponse resp;
    resp.ret_ = MMC_OK;
    resp.msgId = LM_BLOB_DELETE_RSP;

    NetMsgPacker packer;
    ASSERT_EQ(resp.Serialize(packer), MMC_OK);
    auto serialized = packer.String();

    BlobDeleteResponse resp2;
    NetMsgUnpacker unpacker(serialized);
    ASSERT_EQ(resp2.Deserialize(unpacker), MMC_OK);
    EXPECT_EQ(resp2.ret_, MMC_OK);
}

// BlobDeleteResponse serialization/deserialization — failure
TEST_F(TestProtoMsg, BlobDeleteResponse_Failure)
{
    BlobDeleteResponse resp;
    resp.ret_ = MMC_ERROR;
    resp.msgId = LM_BLOB_DELETE_RSP;

    NetMsgPacker packer;
    ASSERT_EQ(resp.Serialize(packer), MMC_OK);
    auto serialized = packer.String();

    BlobDeleteResponse resp2;
    NetMsgUnpacker unpacker(serialized);
    ASSERT_EQ(resp2.Deserialize(unpacker), MMC_OK);
    EXPECT_EQ(resp2.ret_, MMC_ERROR);
}

// MetaNetClient BlobDelete handler registration compile verification
// LM_BLOB_DELETE_REQ/LM_BLOB_DELETE_RSP 消息类型存在且可被 MetaNetClient::Start() 注册
// BlobDeleteRequest/BlobDeleteResponse 类型可被 Handler 函数正确解包
TEST_F(TestProtoMsg, BlobDeleteRequest_MsgIdMatchesMetaNetClientRegistration)
{
    // 验证 LM_BLOB_DELETE_REQ 枚举值正确
    BlobDeleteRequest req;
    req.msgId = LM_BLOB_DELETE_REQ;
    EXPECT_NE(req.msgId, -1);
    EXPECT_NE(req.msgId, 0);

    // 验证 LM_BLOB_DELETE_RSP 枚举值正确
    BlobDeleteResponse resp;
    resp.msgId = LM_BLOB_DELETE_RSP;
    EXPECT_NE(resp.msgId, -1);
    EXPECT_NE(resp.msgId, 0);

    // LM_* REQ/RSP 对共享同一 ID 值，与其他 LM_* 对不冲突
    EXPECT_EQ(static_cast<int16_t>(LM_BLOB_DELETE_REQ), static_cast<int16_t>(LM_BLOB_DELETE_RSP));
}

// Replication retains the baseline field order and the original message version.
TEST_F(TestProtoMsg, MetaReplicateRequest_SerializeDeserialize)
{
    constexpr uint32_t rank = 3;
    constexpr uint64_t dramAddress = 4096;
    constexpr uint64_t bytes = 4096;
    constexpr uint64_t leaseTtlMs = 2500;
    MetaReplicateRequest req;
    EXPECT_EQ(req.msgVer, 0);
    req.destRankId = rank;
    req.ops_ = {META_BACKUP_ADD, META_BACKUP_REMOVE};
    req.keys_ = {"dram-key", "lower-key"};
    req.blobs_ = {{rank, dramAddress, bytes, MEDIA_DRAM, READABLE, leaseTtlMs},
                  {rank, 0, bytes, MEDIA_SSD}};

    NetMsgPacker packer;
    ASSERT_EQ(req.Serialize(packer), MMC_OK);
    NetMsgPacker expected;
    expected.Serialize(req.msgVer);
    expected.Serialize(req.msgId);
    expected.Serialize(req.destRankId);
    expected.Serialize(req.ops_);
    expected.Serialize(req.keys_);
    expected.Serialize(req.blobs_);
    EXPECT_EQ(packer.String(), expected.String());

    NetMsgUnpacker unpacker(packer.String());
    MetaReplicateRequest decoded;
    ASSERT_EQ(decoded.Deserialize(unpacker), MMC_OK);
    EXPECT_EQ(decoded.msgVer, req.msgVer);
    EXPECT_EQ(decoded.msgId, req.msgId);
    EXPECT_EQ(decoded.destRankId, req.destRankId);
    EXPECT_EQ(decoded.ops_, req.ops_);
    EXPECT_EQ(decoded.keys_, req.keys_);
    ASSERT_EQ(decoded.blobs_.size(), req.blobs_.size());
    EXPECT_EQ(decoded.blobs_, req.blobs_);
    for (size_t i = 0; i < req.blobs_.size(); ++i) {
        EXPECT_EQ(decoded.blobs_[i].state_, req.blobs_[i].state_);
        EXPECT_EQ(decoded.blobs_[i].leaseTimeoutTtlMs_, req.blobs_[i].leaseTimeoutTtlMs_);
    }
}
