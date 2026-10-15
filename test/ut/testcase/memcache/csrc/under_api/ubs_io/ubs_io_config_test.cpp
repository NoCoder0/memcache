/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2026. All rights reserved.
 * MemCache_Hybrid is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#include <cstdlib>
#include <dlfcn.h>
#include <gtest/gtest.h>
#include "mmc_ubs_io_proxy.h"

namespace ock {
namespace mmc {
namespace {
constexpr int32_t kHostDevice = -1;
constexpr int32_t kInitFailure = 3;
constexpr uint32_t kOneCall = 1;
constexpr int kOverwriteEnvironment = 1;

class UbsIoConfigTest : public ::testing::Test {
protected:
    using ConfigureFunc = void (*)(int32_t, bool);
    using CountFunc = uint32_t (*)();

    void SetUp() override
    {
        oldPath_ = SafeGetEnv("UBSIO_CONFIG_PATH");
        hadPath_ = std::getenv("UBSIO_CONFIG_PATH") != nullptr;
        DlUbsioApi::CleanupLibrary();
        handle_ = dlopen("libubsio_kvc.so.1", RTLD_NOW | RTLD_LOCAL);
        ASSERT_NE(handle_, nullptr);
        configure_ = reinterpret_cast<ConfigureFunc>(dlsym(handle_, "UbsioMockConfigureInit"));
        registerCalls_ = reinterpret_cast<CountFunc>(dlsym(handle_, "UbsioMockGetRegisterCalls"));
        initCalls_ = reinterpret_cast<CountFunc>(dlsym(handle_, "UbsioMockGetInitCalls"));
        ASSERT_NE(configure_, nullptr);
        ASSERT_NE(registerCalls_, nullptr);
        ASSERT_NE(initCalls_, nullptr);
        configure_(MMC_OK, false);
        proxy_ = new MmcUbsIoProxy("config-test");
        proxy_->SetMetaEventCallback([](int, const std::vector<std::string> &) {});
    }

    void TearDown() override
    {
        if (proxy_ != nullptr) {
            proxy_->DestroyUbsIo();
        }
        DlUbsioApi::CleanupLibrary();
        if (configure_ != nullptr) {
            configure_(MMC_OK, false);
        }
        if (handle_ != nullptr) {
            dlclose(handle_);
        }
        if (hadPath_) {
            setenv("UBSIO_CONFIG_PATH", oldPath_.c_str(), kOverwriteEnvironment);
        } else {
            unsetenv("UBSIO_CONFIG_PATH");
        }
    }

    MmcUbsIoProxyPtr proxy_;
    void *handle_ = nullptr;
    ConfigureFunc configure_ = nullptr;
    CountFunc registerCalls_ = nullptr;
    CountFunc initCalls_ = nullptr;
    std::string oldPath_;
    bool hadPath_ = false;
};

TEST_F(UbsIoConfigTest, ExplicitPathKeepsOriginalEnvironmentBridge)
{
    const char *sdkPath = "/tmp/ubsio-owned.conf";
    const char *envPath = "/tmp/previous.conf";
    ASSERT_EQ(setenv("UBSIO_CONFIG_PATH", envPath, kOverwriteEnvironment), 0);
    ASSERT_EQ(proxy_->InitUbsIo(kHostDevice, sdkPath), MMC_OK);
    EXPECT_STREQ(std::getenv("UBSIO_CONFIG_PATH"), sdkPath);
    EXPECT_EQ(initCalls_(), kOneCall);
}

TEST_F(UbsIoConfigTest, EmptyPathLeavesSdkConfigurationUnchanged)
{
    ASSERT_EQ(setenv("UBSIO_CONFIG_PATH", "/tmp/ubsio-owned.conf", kOverwriteEnvironment), 0);
    ASSERT_EQ(proxy_->InitUbsIo(kHostDevice), MMC_OK);
    EXPECT_STREQ(std::getenv("UBSIO_CONFIG_PATH"), "/tmp/ubsio-owned.conf");
}

TEST_F(UbsIoConfigTest, CallbackIsRegisteredBeforeInit)
{
    configure_(MMC_OK, true);
    bool callbackSeen = false;
    proxy_->SetMetaEventCallback([this, &callbackSeen](int, const std::vector<std::string> &) {
        callbackSeen = true;
        EXPECT_EQ(registerCalls_(), kOneCall);
    });
    ASSERT_EQ(proxy_->InitUbsIo(kHostDevice), MMC_OK);
    EXPECT_TRUE(callbackSeen);
    EXPECT_EQ(proxy_->InitUbsIo(kHostDevice), MMC_OK);
    EXPECT_EQ(initCalls_(), kOneCall);
}

TEST_F(UbsIoConfigTest, InitFailureDoesNotMarkProxyReady)
{
    configure_(kInitFailure, false);
    EXPECT_EQ(proxy_->InitUbsIo(kHostDevice), MMC_ERROR);
    EXPECT_FALSE(proxy_->IsReady());
    EXPECT_EQ(initCalls_(), kOneCall);
}
} // namespace
} // namespace mmc
} // namespace ock
