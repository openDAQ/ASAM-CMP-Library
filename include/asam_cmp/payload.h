#pragma once

#include <vector>

#include <asam_cmp/common.h>
#include <asam_cmp/cmp_header.h>
#include <asam_cmp/message_header.h>
#include <asam_cmp/payload_type.h>

BEGIN_NAMESPACE_ASAM_CMP

class Payload
{
private:
    using MessageType = CmpHeader::MessageType;

public:
    Payload(const PayloadType type, const uint8_t* data, const size_t size);
    Payload(const Payload& other);
    Payload(Payload&& other) noexcept;

    Payload& operator=(const Payload& other);
    Payload& operator=(Payload&& other) noexcept;
    friend bool operator==(const Payload& lhs, const Payload& rhs) noexcept;

    virtual ~Payload() = default;

    PayloadType getType() const;
    size_t getSize() const;
    const uint8_t* getRawPayload() const;

protected:
    std::vector<uint8_t> payloadData;
    PayloadType type{PayloadType::invalid};

private:
    friend void swap(Payload& lhs, Payload& rhs) noexcept;
};

END_NAMESPACE_ASAM_CMP
