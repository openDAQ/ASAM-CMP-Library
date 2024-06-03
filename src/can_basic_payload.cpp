#include <asam_cmp/can_basic_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

uint16_t CanBasicPayload::Header::getFlags() const
{
    return swapEndian(flags);
}

void CanBasicPayload::Header::setFlags(const uint16_t newFlags)
{
    flags = swapEndian(newFlags);
}

bool CanBasicPayload::Header::getFlag(const Flags mask) const
{
    return (getFlags() & static_cast<uint16_t>(mask)) != 0;
}

void CanBasicPayload::Header::setFlag(const Flags mask, const bool value)
{
    if (value)
        setFlags(getFlags() | static_cast<uint16_t>(mask));
    else
        setFlags(getFlags() & ~static_cast<uint16_t>(mask));
}

uint32_t CanBasicPayload::Header::getId() const
{
    return swapEndian(id & idMask);
}

void CanBasicPayload::Header::setId(const uint32_t newId)
{
    id &= ~idMask;
    id |= swapEndian(newId);
}

bool CanBasicPayload::Header::getRsvd() const
{
    return (id & rsvdMask) != 0;
}

void CanBasicPayload::Header::setRsvd(bool rsvd)
{
    id = rsvd ? id | rsvdMask : id & ~rsvdMask;
}

bool CanBasicPayload::Header::getRtrRrs() const
{
    return (id & rtrMask) != 0;
}

void CanBasicPayload::Header::setRtrRrs(const bool rtrRrs)
{
    id = rtrRrs ? id | rtrMask : id & ~rtrMask;
}

bool CanBasicPayload::Header::getIde() const
{
    return (id & ideMask) != 0;
}

void CanBasicPayload::Header::setIde(const bool ide)
{
    id = ide ? id | ideMask : id & ~ideMask;
}

uint16_t CanBasicPayload::Header::getCrc() const
{
    return static_cast<uint16_t>(swapEndian(crc & crcMask));
}

void CanBasicPayload::Header::setCrc(const uint16_t newCrc)
{
    crc &= ~crcMask;
    crc |= swapEndian(static_cast<uint32_t>(newCrc));
}

bool CanBasicPayload::Header::getCrcSupport() const
{
    return (crc & crcSupportMask) != 0;
}

void CanBasicPayload::Header::setCrcSupport(const bool support)
{
    crc = support ? crc | crcSupportMask : id & ~crcSupportMask;
}

uint32_t CanBasicPayload::Header::getCrcSbc() const
{
    return swapEndian(crc & crcSbcMask);
}

void CanBasicPayload::Header::setCrcSbc(const uint32_t newCrcSbc)
{
    crc &= ~crcSbcMask;
    crc |= swapEndian(newCrcSbc);
}

uint8_t CanBasicPayload::Header::getSbc() const
{
    return static_cast<uint8_t>(swapEndian(crc & crcSbcSbcMask) >> crcSbcSbcShift);
}

void CanBasicPayload::Header::setSbc(const uint8_t sbc)
{
    crc &= ~crcSbcSbcMask;
    crc |= swapEndian(static_cast<uint32_t>(sbc) << crcSbcSbcShift);
}

bool CanBasicPayload::Header::getSbcParity() const
{
    return (crc & crcSbcParityMask) != 0;
}

void CanBasicPayload::Header::setSbcParity(const bool parity)
{
    crc = parity ? crc | crcSbcParityMask : id & ~crcSbcParityMask;
}

bool CanBasicPayload::Header::getSbcSupport() const
{
    return (crc & crcSbcSupportMask) != 0;
}

void CanBasicPayload::Header::setSbcSupport(const bool support)
{
    crc = support ? crc | crcSbcSupportMask : id & ~crcSbcSupportMask;
}

uint16_t CanBasicPayload::Header::getErrorPosition() const
{
    return swapEndian(errorPosition);
}

void CanBasicPayload::Header::setErrorPosition(const uint16_t position)
{
    errorPosition = swapEndian(position);
}

bool CanBasicPayload::Header::hasError() const
{
    return ((flags & errorMask) != 0) || (errorPosition != 0);
}

uint8_t CanBasicPayload::Header::getDlc() const
{
    return dlc;
}

void CanBasicPayload::Header::setDlc(const uint8_t newDlc)
{
    dlc = newDlc;
}

uint8_t CanBasicPayload::Header::getDataLength() const
{
    return dataLength;
}

void CanBasicPayload::Header::setDataLength(const uint8_t length)
{
    dataLength = length;
}

uint16_t CanBasicPayload::getFlags() const
{
    return getHeader()->getFlags();
}

void CanBasicPayload::setFlags(const uint16_t flags)
{
    getHeader()->setFlags(flags);
}

bool CanBasicPayload::getFlag(Flags mask) const
{
    return getHeader()->getFlag(mask);
}

void CanBasicPayload::setFlag(Flags mask, bool value)
{
    getHeader()->setFlag(mask, value);
}

uint32_t CanBasicPayload::getId() const
{
    return getHeader()->getId();
}

void CanBasicPayload::setId(const uint32_t id)
{
    getHeader()->setId(id);
}

bool CanBasicPayload::getRsvd() const
{
    return getHeader()->getRsvd();
}

void CanBasicPayload::setRsvd(const bool rsvd)
{
    getHeader()->setRsvd(rsvd);
}

bool CanBasicPayload::getIde() const
{
    return getHeader()->getIde();
}

void CanBasicPayload::setIde(const bool ide)
{
    getHeader()->setIde(ide);
}

bool CanBasicPayload::getCrcSupport() const
{
    return getHeader()->getCrcSupport();
}

void CanBasicPayload::setCrcSupport(const bool support)
{
    getHeader()->setCrcSupport(support);
}

uint16_t CanBasicPayload::getErrorPosition() const
{
    return getHeader()->getErrorPosition();
}

void CanBasicPayload::setErrorPosition(const uint16_t position)
{
    getHeader()->setErrorPosition(position);
}

uint8_t CanBasicPayload::getDlc() const
{
    return getHeader()->getDlc();
}

uint8_t CanBasicPayload::getDataLength() const
{
    return getHeader()->getDataLength();
}

const uint8_t* CanBasicPayload::getData() const
{
    return payloadData.data() + sizeof(Header);
}

bool CanBasicPayload::isValidPayload(const uint8_t* data, const size_t size)
{
    auto header = reinterpret_cast<const Header*>(data);
    return (size >= sizeof(Header) && !header->hasError() && header->getDataLength() <= size - sizeof(Header));
}

CanBasicPayload::CanBasicPayload(const Type type, const uint8_t* data, const size_t size)
    : Payload(type, data, size)
{
}

const CanBasicPayload::Header* CanBasicPayload::getHeader() const
{
    return reinterpret_cast<const Header*>(payloadData.data());
}

CanBasicPayload::Header* CanBasicPayload::getHeader()
{
    return reinterpret_cast<Header*>(payloadData.data());
}

END_NAMESPACE_ASAM_CMP
