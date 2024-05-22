#pragma once

#include <cstdint>

#define BEGIN_NAMESPACE_ASAM_CMP namespace ASAM { namespace CMP {
#define END_NAMESPACE_ASAM_CMP }}

BEGIN_NAMESPACE_ASAM_CMP

inline uint8_t swapEndian(uint8_t value)
{
    return value;
}

inline uint16_t swapEndian(uint16_t value)
{
    return ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
}

inline uint32_t swapEndian(uint32_t value)
{
    return ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) | ((value & 0x000000FF) << 24);
}

END_NAMESPACE_ASAM_CMP
