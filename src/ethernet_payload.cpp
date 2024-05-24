#include <asam_cmp/ethernet_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

uint16_t EthernetPayload::Header::getFlags() const
{
    return swapEndian(flags);
}

void EthernetPayload::Header::setFlags(uint16_t newFlags)
{
    flags = swapEndian(newFlags);
}

uint16_t EthernetPayload::Header::getDataLength() const
{
    return swapEndian(dataLength);
}

void EthernetPayload::Header::increaseDataLength(uint16_t length)
{
    dataLength = swapEndian(static_cast<uint16_t>(swapEndian(dataLength) + length));
}

EthernetPayload::EthernetPayload(const uint8_t* data, const size_t size)
{
    payloadData.resize(size);
    memcpy(payloadData.data(), data, size);
    type = Type::ethernet;
}

bool EthernetPayload::addSegment(const uint8_t* data, const size_t size)
{
    const auto header = reinterpret_cast<const Header*>(data);
    if (size < header->getDataLength() + sizeof(Header))
        return false;

    const auto oldPayloadSize = payloadData.size();
    const auto newDataSize = header->getDataLength();
    payloadData.resize(oldPayloadSize + newDataSize);
    memcpy(payloadData.data() + oldPayloadSize, data + sizeof(Header), newDataSize);

    auto payloadHeader = reinterpret_cast<Header*>(payloadData.data());
    payloadHeader->increaseDataLength(newDataSize);

    return true;
}

uint16_t EthernetPayload::getDataLength() const
{
    return reinterpret_cast<const Header*>(payloadData.data())->getDataLength();
}

const uint8_t* EthernetPayload::getData() const
{
    return payloadData.data() + sizeof(Header);
}

bool EthernetPayload::isValidPayload(const uint8_t* data, const size_t size)
{
    auto header = reinterpret_cast<const Header*>(data);
    return (size >= sizeof(Header) && (header->getFlags() & errorMask) == 0 && header->getDataLength() <= size - sizeof(Header));
}

END_NAMESPACE_ASAM_CMP
