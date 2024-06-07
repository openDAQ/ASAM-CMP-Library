#pragma once

#include <cstdint>
#include <type_traits>

#define BEGIN_NAMESPACE_ASAM_CMP namespace ASAM { namespace CMP {
#define END_NAMESPACE_ASAM_CMP }}

BEGIN_NAMESPACE_ASAM_CMP

constexpr uint8_t swapEndian(const uint8_t value)
{
    return value;
}

constexpr uint16_t swapEndian(const uint16_t value)
{
    return ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
}

constexpr uint32_t swapEndian(const uint32_t value)
{
    return ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) | ((value & 0x000000FF) << 24);
}

inline float swapEndian(const float inFloat)
{
    float ret{0.f};
    auto inPtr = reinterpret_cast<const char*>(&inFloat);
    auto outPtr = reinterpret_cast<char*>(&ret);

    outPtr[0] = inPtr[3];
    outPtr[1] = inPtr[2];
    outPtr[2] = inPtr[1];
    outPtr[3] = inPtr[0];

    return ret;
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
