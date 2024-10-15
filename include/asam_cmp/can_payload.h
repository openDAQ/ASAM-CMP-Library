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

#include <asam_cmp/can_payload_base.h>
#include <asam_cmp/common.h>

BEGIN_NAMESPACE_ASAM_CMP

class CanPayload : public CanPayloadBase
{
public:
    CanPayload();
    CanPayload(const uint8_t* data, const size_t size);

    bool getRtr() const;
    void setRtr(const bool rtr);
    uint16_t getCrc() const;
    void setCrc(const uint16_t crc);
};

END_NAMESPACE_ASAM_CMP
