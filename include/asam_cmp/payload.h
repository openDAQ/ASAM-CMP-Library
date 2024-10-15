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

#include <cstddef>
#include <vector>
#include <cstring>

#include <asam_cmp/cmp_header.h>
#include <asam_cmp/common.h>
#include <asam_cmp/message_header.h>
#include <asam_cmp/payload_type.h>
#include <memory>

BEGIN_NAMESPACE_ASAM_CMP

class Payload
{
private:
    using MessageType = CmpHeader::MessageType;

public:
    Payload(const PayloadType type, const uint8_t* data, const size_t size);
    Payload(const Payload& other) = default;
    Payload(Payload&& other) = default;

    Payload& operator=(const Payload& other) = default;
    Payload& operator=(Payload&& other) = default;
    friend bool operator==(const Payload& lhs, const Payload& rhs) noexcept;

    virtual ~Payload() = default;

public:
    bool isValid() const;

    MessageType getMessageType() const;
    void setMessageType(const MessageType newType);
    uint8_t getRawPayloadType() const;
    void setRawPayloadType(const uint8_t newType);
    PayloadType getType() const;
    void setType(const PayloadType newType);
    size_t getLength() const;
    const uint8_t* getRawPayload() const;

protected:
    Payload(const PayloadType type, const size_t size);

    template <typename Header>
    void setData(const uint8_t* data, const size_t size);

protected:
    std::vector<uint8_t> payloadData;
    PayloadType type{PayloadType::invalid};
};

template <typename Header>
inline void Payload::setData(const uint8_t* data, const size_t size)
{
    payloadData.resize(sizeof(Header) + size);
    memcpy(payloadData.data() + sizeof(Header), data, size);
}

END_NAMESPACE_ASAM_CMP
