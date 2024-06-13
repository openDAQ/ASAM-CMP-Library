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

    if (lhs.getSize() != rhs.getSize())
        return false;

    const uint8_t* lhsRaw = lhs.getRawPayload();
    const uint8_t* rhsRaw = rhs.getRawPayload();

    if (lhsRaw == rhsRaw)
        return false;

    for (size_t i = 0; i < lhs.getSize(); ++i)
        if (lhsRaw[i] != rhsRaw[i])
            return false;

    return true;
}

PayloadType Payload::getType() const
{
    return type;
}

size_t Payload::getSize() const
{
    return payloadData.size();
}

const uint8_t* Payload::getRawPayload() const
{
    return payloadData.data();
}

END_NAMESPACE_ASAM_CMP
