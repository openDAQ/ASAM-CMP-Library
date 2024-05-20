#include <asam_cmp/can_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

CanPayload::CanPayload(const uint8_t* data, const size_t size)
    : Payload(Payload::Type::can, data, size)
{
}

uint32_t CanPayload::getId() const
{
    return swapEndian(reinterpret_cast<const Header*>(payloadData.data())->id);
}

uint8_t CanPayload::getDataLength() const
{
    return reinterpret_cast<const Header*>(payloadData.data())->dataLength;
}

const uint8_t* CanPayload::getData() const
{
    return payloadData.data() + sizeof(Header);
}

END_NAMESPACE_ASAM_CMP
