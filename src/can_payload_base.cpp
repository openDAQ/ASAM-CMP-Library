#include <asam_cmp/can_payload_base.h>

BEGIN_NAMESPACE_ASAM_CMP

uint16_t CanPayloadBase::Header::getFlags() const
{
    return swapEndian(flags);
}

void CanPayloadBase::Header::setFlags(const uint16_t newFlags)
{
    flags = swapEndian(newFlags);
}

bool CanPayloadBase::Header::getFlag(const Flags mask) const
{
    return (getFlags() & static_cast<uint16_t>(mask)) != 0;
}

void CanPayloadBase::Header::setFlag(const Flags mask, const bool value)
{
    if (value)
        setFlags(getFlags() | static_cast<uint16_t>(mask));
    else
        setFlags(getFlags() & ~static_cast<uint16_t>(mask));
}

uint32_t CanPayloadBase::Header::getId() const
{
    return swapEndian(id & idMask);
}

void CanPayloadBase::Header::setId(const uint32_t newId)
{
    id &= ~idMask;
    id |= swapEndian(newId);
}

bool CanPayloadBase::Header::getRsvd() const
{
    return (id & rsvdMask) != 0;
}

void CanPayloadBase::Header::setRsvd(bool rsvd)
{
    id = rsvd ? id | rsvdMask : id & ~rsvdMask;
}

bool CanPayloadBase::Header::getRtrRrs() const
{
    return (id & rtrMask) != 0;
}

void CanPayloadBase::Header::setRtrRrs(const bool rtrRrs)
{
    id = rtrRrs ? id | rtrMask : id & ~rtrMask;
}

bool CanPayloadBase::Header::getIde() const
{
    return (id & ideMask) != 0;
}

void CanPayloadBase::Header::setIde(const bool ide)
{
    id = ide ? id | ideMask : id & ~ideMask;
}

uint16_t CanPayloadBase::Header::getCrc() const
{
    return static_cast<uint16_t>(swapEndian(crc & crcMask));
}

void CanPayloadBase::Header::setCrc(const uint16_t newCrc)
{
    crc &= ~crcMask;
    crc |= swapEndian(static_cast<uint32_t>(newCrc));
}

bool CanPayloadBase::Header::getCrcSupport() const
{
    return (crc & crcSupportMask) != 0;
}

void CanPayloadBase::Header::setCrcSupport(const bool support)
{
    crc = support ? crc | crcSupportMask : id & ~crcSupportMask;
}

uint32_t CanPayloadBase::Header::getCrcSbc() const
{
    return swapEndian(crc & crcSbcMask);
}

void CanPayloadBase::Header::setCrcSbc(const uint32_t newCrcSbc)
{
    crc &= ~crcSbcMask;
    crc |= swapEndian(newCrcSbc);
}

uint8_t CanPayloadBase::Header::getSbc() const
{
    return static_cast<uint8_t>(swapEndian(crc & crcSbcSbcMask) >> crcSbcSbcShift);
}

void CanPayloadBase::Header::setSbc(const uint8_t sbc)
{
    crc &= ~crcSbcSbcMask;
    crc |= swapEndian(static_cast<uint32_t>(sbc) << crcSbcSbcShift);
}

bool CanPayloadBase::Header::getSbcParity() const
{
    return (crc & crcSbcParityMask) != 0;
}

void CanPayloadBase::Header::setSbcParity(const bool parity)
{
    crc = parity ? crc | crcSbcParityMask : id & ~crcSbcParityMask;
}

bool CanPayloadBase::Header::getSbcSupport() const
{
    return (crc & crcSbcSupportMask) != 0;
}

void CanPayloadBase::Header::setSbcSupport(const bool support)
{
    crc = support ? crc | crcSbcSupportMask : id & ~crcSbcSupportMask;
}

uint16_t CanPayloadBase::Header::getErrorPosition() const
{
    return swapEndian(errorPosition);
}

void CanPayloadBase::Header::setErrorPosition(const uint16_t position)
{
    errorPosition = swapEndian(position);
}

bool CanPayloadBase::Header::hasError() const
{
    return ((flags & errorMask) != 0) || (errorPosition != 0);
}

uint8_t CanPayloadBase::Header::getDlc() const
{
    return dlc;
}

void CanPayloadBase::Header::setDlc(const uint8_t newDlc)
{
    dlc = newDlc;
}

uint8_t CanPayloadBase::Header::getDataLength() const
{
    return dataLength;
}

void CanPayloadBase::Header::setDataLength(const uint8_t length)
{
    dataLength = length;
}

uint16_t CanPayloadBase::getFlags() const
{
    return getHeader()->getFlags();
}

void CanPayloadBase::setFlags(const uint16_t flags)
{
    getHeader()->setFlags(flags);
}

bool CanPayloadBase::getFlag(Flags mask) const
{
    return getHeader()->getFlag(mask);
}

void CanPayloadBase::setFlag(Flags mask, bool value)
{
    getHeader()->setFlag(mask, value);
}

uint32_t CanPayloadBase::getId() const
{
    return getHeader()->getId();
}

void CanPayloadBase::setId(const uint32_t id)
{
    getHeader()->setId(id);
}

bool CanPayloadBase::getRsvd() const
{
    return getHeader()->getRsvd();
}

void CanPayloadBase::setRsvd(const bool rsvd)
{
    getHeader()->setRsvd(rsvd);
}

bool CanPayloadBase::getIde() const
{
    return getHeader()->getIde();
}

void CanPayloadBase::setIde(const bool ide)
{
    getHeader()->setIde(ide);
}

bool CanPayloadBase::getCrcSupport() const
{
    return getHeader()->getCrcSupport();
}

void CanPayloadBase::setCrcSupport(const bool support)
{
    getHeader()->setCrcSupport(support);
}

uint16_t CanPayloadBase::getErrorPosition() const
{
    return getHeader()->getErrorPosition();
}

void CanPayloadBase::setErrorPosition(const uint16_t position)
{
    getHeader()->setErrorPosition(position);
}

uint8_t CanPayloadBase::getDlc() const
{
    return getHeader()->getDlc();
}

uint8_t CanPayloadBase::getDataLength() const
{
    return getHeader()->getDataLength();
}

const uint8_t* CanPayloadBase::getData() const
{
    return payloadData.data() + sizeof(Header);
}

bool CanPayloadBase::isValidPayload(const uint8_t* data, const size_t size)
{
    auto header = reinterpret_cast<const Header*>(data);
    return (size >= sizeof(Header) && !header->hasError() && header->getDataLength() <= size - sizeof(Header));
}

CanPayloadBase::CanPayloadBase(const DataMessageHeader::PayloadType type, const uint8_t* data, const size_t size)
    : Payload(type, data, size)
{
}

const CanPayloadBase::Header* CanPayloadBase::getHeader() const
{
    return reinterpret_cast<const Header*>(payloadData.data());
}

CanPayloadBase::Header* CanPayloadBase::getHeader()
{
    return reinterpret_cast<Header*>(payloadData.data());
}

END_NAMESPACE_ASAM_CMP
