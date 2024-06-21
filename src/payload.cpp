#include <cstring>

#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

Payload::Payload(const PayloadType type, const uint8_t* data, const size_t size)
    : payloadData(size)
    , type(type)
{
    if (size && type != PayloadType::invalid)
        memcpy(payloadData.data(), data, size);
}

bool operator==(const Payload& lhs, const Payload& rhs) noexcept
{
    if (lhs.getType() != rhs.getType())
        return false;

    if (lhs.getLength() != rhs.getLength())
        return false;

    const uint8_t* lhsRaw = lhs.getRawPayload();
    const uint8_t* rhsRaw = rhs.getRawPayload();

    if (lhsRaw == rhsRaw)
        return false;

    for (size_t i = 0; i < lhs.getLength(); ++i)
        if (lhsRaw[i] != rhsRaw[i])
            return false;

    return true;
}

bool Payload::isValid() const
{
    return type.isValid();
}

Payload::MessageType Payload::getMessageType() const
{
    return type.getMessageType();
}

void Payload::setMessageType(const MessageType newType)
{
    type.setMessageType(newType);
}

uint8_t Payload::getRawPayloadType() const
{
    return type.getRawPayloadType();
}

void Payload::setRawPayloadType(const uint8_t newType)
{
    type.setRawPayloadType(newType);
}

PayloadType Payload::getType() const
{
    return type;
}

void Payload::setType(const PayloadType newType)
{
    type = newType;
}

size_t Payload::getLength() const
{
    return payloadData.size();
}

const uint8_t* Payload::getRawPayload() const
{
    return payloadData.data();
}

END_NAMESPACE_ASAM_CMP
