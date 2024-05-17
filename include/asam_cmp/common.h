#pragma once

#include <cstdint>

#define BEGIN_NAMESPACE_ASAM_CMP namespace AsamCmp {
#define END_NAMESPACE_ASAM_CMP }

BEGIN_NAMESPACE_ASAM_CMP

inline uint8_t swap_endian(uint8_t value)
{
    return value;
}

inline uint16_t swap_endian(uint16_t value)
{
    return ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
}

inline uint32_t swap_endian(uint32_t value)
{
    return ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) | ((value & 0x000000FF) << 24);
}

END_NAMESPACE_ASAM_CMP
