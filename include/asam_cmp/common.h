#pragma once

#include <cstdint>
#include <type_traits>

#define BEGIN_NAMESPACE_ASAM_CMP namespace ASAM { namespace CMP {
#define END_NAMESPACE_ASAM_CMP }}

BEGIN_NAMESPACE_ASAM_CMP

constexpr uint8_t swapEndian(uint8_t value)
{
    return value;
}

constexpr uint16_t swapEndian(uint16_t value)
{
    return ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
}

constexpr uint32_t swapEndian(uint32_t value)
{
    return ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) | ((value & 0x000000FF) << 24);
}

constexpr uint64_t swapEndian(uint64_t value)
{
    return (((value & 0xFF00000000000000) >> 56) | ((value & 0x00FF000000000000) >> 40) | ((value & 0x0000FF0000000000) >> 24) |
            ((value & 0x000000FF00000000) >> 8) | ((value & 0x00000000FF000000) << 8) | ((value & 0x0000000000FF0000) << 24) |
            ((value & 0x000000000000FF00) << 40) | ((value & 0x00000000000000FF) << 56));
}

template <typename T>
constexpr typename std::underlying_type<T>::type to_underlying(T value) noexcept
{
    return static_cast<typename std::underlying_type<T>::type>(value);
}

#pragma pack(push, 1)

struct CmpMessageHeader
{
    uint8_t version{1};
    uint8_t reserved{0};
    uint16_t deviceId{0};
    uint8_t messageType{0};
    uint8_t streamId{0};
    uint16_t sequenceCounter{0};
};

struct DataMessageHeader
{
    uint64_t timestamp{0};
    uint32_t interfaceId{0};
    uint8_t flags{0};
    uint8_t payloadType{0};
    uint16_t payloadLength{0};
};

#pragma pack(pop)

END_NAMESPACE_ASAM_CMP
