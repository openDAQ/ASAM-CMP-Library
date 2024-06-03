#include <asam_cmp/ethernet_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

uint16_t EthernetPayload::Header::getFlags() const
{
    return swapEndian(flags);
}

void EthernetPayload::Header::setFlags(const uint16_t newFlags)
{
    flags = swapEndian(newFlags);
}

bool EthernetPayload::Header::getFlag(const Flags mask) const
{
    return (getFlags() & static_cast<uint16_t>(mask)) != 0;
}

void EthernetPayload::Header::setFlag(const Flags mask, const bool value)
{
    if (value)
        setFlags(getFlags() | static_cast<uint16_t>(mask));
    else
        setFlags(getFlags() & ~static_cast<uint16_t>(mask));
}

uint16_t EthernetPayload::Header::getDataLength() const
{
    return swapEndian(dataLength);
}

void EthernetPayload::Header::setDataLength(uint16_t newDataLength)
{
    dataLength = swapEndian(newDataLength);
}

EthernetPayload::EthernetPayload(const uint8_t* data, const size_t size)
    : Payload(Type::ethernet, data, size)
{
}

uint16_t EthernetPayload::getFlags() const
{
    return getHeader()->getFlags();
}

void EthernetPayload::setFlags(uint16_t newFlags)
{
    getHeader()->setFlags(newFlags);
}

bool EthernetPayload::getFlag(Flags mask) const
{
    return getHeader()->getFlag(mask);
}

void EthernetPayload::setFlag(Flags mask, bool value)
{
    getHeader()->setFlag(mask, value);
}

uint16_t EthernetPayload::getDataLength() const
{
    return getHeader()->getDataLength();
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

const EthernetPayload::Header* EthernetPayload::getHeader() const
{
    return reinterpret_cast<const Header*>(payloadData.data());
}

EthernetPayload::Header* EthernetPayload::getHeader()
{
    return reinterpret_cast<Header*>(payloadData.data());
}

END_NAMESPACE_ASAM_CMP
