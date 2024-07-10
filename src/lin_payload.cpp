#include <asam_cmp/lin_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

uint16_t LinPayload::Header::getFlags() const
{
    return swapEndian(flags);
}

void LinPayload::Header::setFlags(const uint16_t newFlags)
{
    flags = swapEndian(newFlags);
}

bool LinPayload::Header::getFlag(const Flags mask) const
{
    return (getFlags() & static_cast<uint16_t>(mask)) != 0;
}

void LinPayload::Header::setFlag(const Flags mask, const bool value)
{
    if (value)
        setFlags(getFlags() | static_cast<uint16_t>(mask));
    else
        setFlags(getFlags() & ~static_cast<uint16_t>(mask));
}

uint8_t LinPayload::Header::getLinId() const
{
    return pid & linIdMask;
}

void LinPayload::Header::setLinId(const uint8_t id)
{
    pid &= ~linIdMask;
    pid |= (id & linIdMask);
}

uint8_t LinPayload::Header::getParityBits() const
{
    return (pid & parityMask) >> parityShift;
}

void LinPayload::Header::setParityBits(const uint8_t parity)
{
    pid &= ~parityMask;
    pid |= parity << parityShift;
}

uint8_t LinPayload::Header::getChecksum() const
{
    return checksum;
}

void LinPayload::Header::setChecksum(const uint8_t newChecksum)
{
    checksum = newChecksum;
}

uint8_t LinPayload::Header::getDataLength() const
{
    return dataLength;
}

void LinPayload::Header::setDataLength(const uint8_t length)
{
    dataLength = length;
}

LinPayload::LinPayload()
    : Payload(PayloadType::lin, sizeof(Header))
{
}

LinPayload::LinPayload(const uint8_t* data, const size_t size)
    : Payload(PayloadType::lin, data, size)
{
}

uint16_t LinPayload::getFlags() const
{
    return getHeader()->getFlags();
}

void LinPayload::setFlags(uint16_t flags)
{
    getHeader()->setFlags(flags);
}

bool LinPayload::getFlag(Flags mask) const
{
    return getHeader()->getFlag(mask);
}

void LinPayload::setFlag(Flags mask, bool value)
{
    getHeader()->setFlag(mask, value);
}

uint8_t LinPayload::getLinId() const
{
    return getHeader()->getLinId();
}

void LinPayload::setLinId(const uint8_t id)
{
    getHeader()->setLinId(id);
}

uint8_t LinPayload::getParityBits() const
{
    return getHeader()->getParityBits();
}

void LinPayload::setParityBits(const uint8_t parity)
{
    getHeader()->setParityBits(parity);
}

uint8_t LinPayload::getChecksum() const
{
    return getHeader()->getChecksum();
}

void LinPayload::setChecksum(const uint8_t checksum)
{
    getHeader()->setChecksum(checksum);
}

uint8_t LinPayload::getDataLength() const
{
    return getHeader()->getDataLength();
}

const uint8_t* LinPayload::getData() const
{
    return getDataLength() ? payloadData.data() + sizeof(Header) : nullptr;
}

bool LinPayload::isValidPayload([[maybe_unused]] const uint8_t* data, const size_t size)
{
    return (size >= sizeof(Header));
}

const LinPayload::Header* LinPayload::getHeader() const
{
    return reinterpret_cast<const Header*>(payloadData.data());
}

LinPayload::Header* LinPayload::getHeader()
{
    return reinterpret_cast<Header*>(payloadData.data());
}

END_NAMESPACE_ASAM_CMP
