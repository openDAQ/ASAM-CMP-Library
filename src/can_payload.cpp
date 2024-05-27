#include <asam_cmp/can_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

uint16_t CanPayload::Header::getFlags() const
{
    return swapEndian(flags);
}

void CanPayload::Header::setFlags(const uint16_t newFlags)
{
    flags = swapEndian(newFlags);
}

uint32_t CanPayload::Header::getId() const
{
    return swapEndian(id);
}

void CanPayload::Header::setId(const uint32_t newId)
{
    id = swapEndian(newId);
}

uint16_t CanPayload::Header::getErrorPosition() const
{
    return swapEndian(errorPosition);
}

void CanPayload::Header::setErrorPosition(const uint16_t position)
{
    errorPosition = swapEndian(position);
}

bool CanPayload::Header::hasError() const
{
    return ((flags & errorMask) != 0) || (errorPosition != 0);
}

uint8_t CanPayload::Header::getDataLength() const
{
    return dataLength;
}

void CanPayload::Header::setDataLength(const uint8_t length)
{
    dataLength = length;
}

CanPayload::CanPayload(const uint8_t* data, const size_t size)
{
    payloadData.resize(size);
    memcpy(payloadData.data(), data, size);
    type = Type::can;
}

uint32_t CanPayload::getId() const
{
    return reinterpret_cast<const Header*>(payloadData.data())->getId();
}

uint8_t CanPayload::getDataLength() const
{
    return reinterpret_cast<const Header*>(payloadData.data())->getDataLength();
}

const uint8_t* CanPayload::getData() const
{
    return payloadData.data() + sizeof(Header);
}

bool CanPayload::isValidPayload(const uint8_t* data, const size_t size)
{
    auto header = reinterpret_cast<const Header*>(data);
    return (size >= sizeof(Header) && !header->hasError() && header->getDataLength() <= size - sizeof(Header));
}

END_NAMESPACE_ASAM_CMP
