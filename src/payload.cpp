#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

Payload::Payload(const Type type, const uint8_t* data, const size_t size)
    : type(type)
    , payloadData(size)
{
    if (size && type != Type::invalid)
        memcpy(payloadData.data(), data, size);
}

Payload::Payload(const Payload& other) noexcept
    : type(other.type)
{
    payloadData.resize(other.payloadData.size());
    memcpy(payloadData.data(), other.payloadData.data(), payloadData.size());
}

Payload::Payload(Payload&& other) noexcept
    : Payload()
{
    swap(*this, other);
}

Payload::Type Payload::getType() const
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

void swap(Payload& lhs, Payload& rhs) noexcept
{
    std::swap(lhs.type, rhs.type);
    std::swap(lhs.payloadData, rhs.payloadData);
}

Payload& Payload::operator=(const Payload& other) noexcept
{
    if (!(*this == other))
    {
        Payload tmp(other);
        swap(*this, tmp);
    }
    return *this;
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

Payload& Payload::operator=(Payload&& other) noexcept
{

    swap(*this, other);
    return *this;
}


END_NAMESPACE_ASAM_CMP
