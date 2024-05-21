#include <asam_cmp/can_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

CanPayload::CanPayload(const uint8_t* data, const size_t size)
{
    payloadData.resize(size);
    memcpy(payloadData.data(), data, size);
    type = Type::can;
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

bool CanPayload::isValidPayload(const uint8_t* data, const size_t size)
{
    auto header = reinterpret_cast<const Header*>(data);
    return (size >= sizeof(Header) && (header->flags & errorMask) == 0 && header->errorPosition == 0 &&
            header->dataLength <= size - sizeof(Header));
}

END_NAMESPACE_ASAM_CMP
