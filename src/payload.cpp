#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

Payload::Payload(const Type type, const uint8_t* data, const size_t size)
    : type(type)
    , payloadData(size)
{
    if (size)
        memcpy(payloadData.data(), data, size);
}

Payload::Type Payload::getType() const
{
    return type;
}

END_NAMESPACE_ASAM_CMP
