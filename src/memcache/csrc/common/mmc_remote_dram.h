/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
 * MemCache_Hybrid is licensed under Mulan PSL v2.
 */
#ifndef MMC_REMOTE_DRAM_H
#define MMC_REMOTE_DRAM_H
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <random>
#include <string>
#include <vector>

namespace ock {
namespace mmc {
inline bool RemoteDramEnabled()
{
    const char *mode = std::getenv("UBSIO_KVC_MODE");
    return mode != nullptr && std::strcmp(mode, "remote_dram_tcp") == 0;
}

inline uint64_t NewLowerGeneration()
{
    // Process epoch plus monotonic sequence: never reuse an address on retry/rebuild.
    static std::atomic<uint64_t> sequence{[]() -> uint64_t {
        std::random_device random;
        return (static_cast<uint64_t>(random()) << 32) ^ random() ^
               static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
    }()};
    auto value = sequence.fetch_add(1);
    while (value == 0 || value == UINT64_MAX) value = sequence.fetch_add(1);
    return value;
}

template<typename Desc>
std::string LowerStorageKey(const std::string &key, const Desc &desc)
{
    // In TCP mode SSD.gva_ is an opaque generation, never a memory address.
    if (!RemoteDramEnabled() || desc.gva_ == 0) return key;
    uint64_t hash = 14695981039346656037ULL;
    for (unsigned char c : key) hash = (hash ^ c) * 1099511628211ULL;
    return "__mmc_dram_v1_" + std::to_string(desc.rank_) + "_" + std::to_string(desc.gva_) + "_" + std::to_string(hash);
}

template<typename Map, typename Desc>
bool EraseExactBackup(Map &backups, const std::string &key, const Desc &desc)
{
    auto entry = backups.find(key);
    if (entry == backups.end()) return false;
    auto &values = entry->second;
    const auto match = std::find(values.begin(), values.end(), desc);
    if (match == values.end()) return false;
    values.erase(match);
    if (values.empty()) backups.erase(entry);
    return true;
}

template<typename Descs>
void DropRemoteRecoveryDescriptors(Descs &descs, uint16_t lowerMedia)
{
    // No remote recovery before a durable Meta/LocalService commit protocol exists.
    descs.erase(std::remove_if(descs.begin(), descs.end(), [lowerMedia](const auto &desc) {
        return desc.mediaType_ == lowerMedia;
    }), descs.end());
}
} // namespace mmc
} // namespace ock
#endif
