/*
 * Copyright 2022-2024 openDAQ d.o.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#pragma once

#include <cstdint>
#include <type_traits>

#define BEGIN_NAMESPACE_ASAM_CMP namespace ASAM { namespace CMP {
#define END_NAMESPACE_ASAM_CMP }}

#define BEGIN_NAMESPACE_TECMP namespace TECMP{
#define END_NAMESPACE_TECMP }

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

constexpr uint64_t swapEndian(uint64_t value)
{
    return (((value & 0xFF00000000000000) >> 56) | ((value & 0x00FF000000000000) >> 40) | ((value & 0x0000FF0000000000) >> 24) |
            ((value & 0x000000FF00000000) >> 8) | ((value & 0x00000000FF000000) << 8) | ((value & 0x0000000000FF0000) << 24) |
            ((value & 0x000000000000FF00) << 40) | ((value & 0x00000000000000FF) << 56));
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

END_NAMESPACE_ASAM_CMP
