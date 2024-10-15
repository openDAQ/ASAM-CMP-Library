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

#include <asam_cmp/common.h>
#include <asam_cmp/tecmp_payload.h>

BEGIN_NAMESPACE_TECMP

class CanPayload : public Payload
{
#pragma pack(push, 1)
    class Header
    {
        uint32_t arbId{0};
        uint8_t dlc{0};

    public:
        uint32_t getArbId() const;
        void setArbId(uint32_t newArbId);

        uint8_t getDlc() const;
        void setDlc(uint8_t newDlc);
    };
#pragma pack(pop)
public:
    uint32_t getArbId() const;
    void setArbId(uint32_t newArbId);

    uint8_t getDlc() const;
    void setDlc(uint8_t newDlc);

public:
    CanPayload();
    CanPayload(const uint8_t* data, const size_t size);

    const uint8_t* getData() const;

    uint32_t getCrc() const;

protected:
    const Header* getHeader() const;
    Header* getHeader();
};

END_NAMESPACE_TECMP