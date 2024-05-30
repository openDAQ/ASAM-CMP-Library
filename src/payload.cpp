#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

Payload::Payload(const Type type, const uint8_t* data, const size_t size)
    : type(type)
    , payloadData(size)
{
    if (size && type != Type::invalid)
        memcpy(payloadData.data(), data, size);
}

Payload::Type Payload::getType() const
{
    return type;
}

size_t Payload::getSize() const
{
    return payloadData.size();
}

END_NAMESPACE_ASAM_CMP
