#include <asam_cmp/tecmp_payload.h>

TECMP::Payload::Payload(const TECMP::PayloadType type, const uint8_t* data, const size_t size)
    : payloadData(size)
    , type(type)
{
    if (size && type != PayloadType::invalid)
        memcpy(payloadData.data(), data, size);
}

TECMP::Payload::Payload(const TECMP::PayloadType type, const size_t size)
    : payloadData(size)
    , type(type)
{
}

bool TECMP::operator==(const TECMP::Payload& lhs, const TECMP::Payload& rhs) noexcept
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

bool TECMP::Payload::isValid() const
{
    return type.isValid();
}

TECMP::Payload::MessageType TECMP::Payload::getMessageType() const
{
    return type.getMessageType();
}

void TECMP::Payload::setMessageType(const TECMP::Payload::MessageType newType)
{
    type.setMessageType(newType);
}

uint8_t TECMP::Payload::getRawPayloadType() const
{
    return type.getRawPayloadType();
}

void TECMP::Payload::setRawPayloadType(const uint8_t newType)
{
    type.setRawPayloadType(newType);
}

TECMP::PayloadType TECMP::Payload::getType() const
{
    return type;
}

void TECMP::Payload::setType(const TECMP::PayloadType newType)
{
    type = newType;
}

size_t TECMP::Payload::getLength() const
{
    return payloadData.size();
}

const uint8_t* TECMP::Payload::getRawPayload() const
{
    return payloadData.data();
}