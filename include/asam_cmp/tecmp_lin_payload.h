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

class LinPayload : public Payload
{
#pragma pack(push, 1)
    class Header
    {
        uint8_t pid;
        uint8_t dataLength;

    public:
        uint8_t getPid() const;
        void setPid(uint8_t newPid);

        uint8_t getDataLength() const;
        void setDataLength(uint8_t newLength);
    };
#pragma pack(pop)
public:
    uint8_t getPid() const;
    void setPid(uint8_t newPid);

    uint8_t getDataLength() const;
    void setDataLength(uint8_t newLength);

public:
    LinPayload();
    LinPayload(const uint8_t* data, const size_t size);

    const uint8_t* getData() const;
    void setData(const uint8_t* data, const uint8_t dataLength);

    uint8_t getCrc() const;

protected:
    const Header* getHeader() const;
    Header* getHeader();
};

END_NAMESPACE_TECMP
