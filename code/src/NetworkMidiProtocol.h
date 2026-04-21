#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace netmidi {

// Wire format v2: timestamps captured on the client with std::chrono::steady_clock
// (nanoseconds since steady_clock epoch). The server preserves inter-onset intervals
// by scheduling each message at: anchor + (sender_ns - origin_ns).

inline constexpr std::uint32_t kMagic = 0x314D524Du; // 'M','R','1', 0x31 in LE: MR1\x31
inline constexpr std::uint16_t kVersion = 2;

#pragma pack(push, 1)
struct PacketV2 {
    std::uint32_t magic{};
    std::uint16_t version{};
    std::uint16_t reserved{};
    std::uint64_t sender_steady_ns{};
    std::uint8_t num_bytes{};
    std::uint8_t bytes[3]{};
    std::uint8_t pad[4]{};
};
#pragma pack(pop)

static_assert(sizeof(PacketV2) == 24, "PacketV2 size");

inline void pack_v2(PacketV2& out, std::uint64_t sender_steady_ns,
                    const unsigned char* msg, std::size_t len)
{
    out.magic           = kMagic;
    out.version         = kVersion;
    out.reserved        = 0;
    out.sender_steady_ns = sender_steady_ns;
    out.num_bytes       = static_cast<std::uint8_t>(len > 3 ? 3 : len);
    std::memset(out.bytes, 0, sizeof(out.bytes));
    std::memcpy(out.bytes, msg, out.num_bytes);
    std::memset(out.pad, 0, sizeof(out.pad));
}

inline bool unpack_v2(const unsigned char* data, std::size_t len, PacketV2* out)
{
    if (len < sizeof(PacketV2))
        return false;
    std::memcpy(out, data, sizeof(PacketV2));
    if (out->magic != kMagic || out->version != kVersion)
        return false;
    if (out->num_bytes == 0 || out->num_bytes > 3)
        return false;
    return true;
}

} // namespace netmidi
