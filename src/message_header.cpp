#include <asam_cmp/message_header.h>


BEGIN_NAMESPACE_ASAM_CMP

uint64_t MessageHeader::getTimestamp() const
{
    return swapEndian(timestamp);
}

void MessageHeader::setTimestamp(const uint64_t newTimestamp)
{
    timestamp = swapEndian(newTimestamp);
}

uint32_t MessageHeader::getInterfaceId() const
{
    return swapEndian(interfaceId);
}

void MessageHeader::setInterfaceId(const uint32_t id)
{
    interfaceId = swapEndian(id);
}

uint16_t MessageHeader::getVendorId() const
{
    return swapEndian(vendor.vendorId);
}

void MessageHeader::setVendorId(const uint16_t id)
{
    vendor.vendorId = swapEndian(id);
}

uint8_t MessageHeader::getCommonFlags() const
{
    return commonFlags;
}

void MessageHeader::setCommonFlags(const uint8_t newFlags)
{
    commonFlags = newFlags;
}

bool MessageHeader::getCommonFlag(const CommonFlags mask) const
{
    return (commonFlags & static_cast<uint8_t>(mask)) != 0;
}

void MessageHeader::setCommonFlag(const CommonFlags mask, const bool value)
{
    commonFlags = value ? (commonFlags | static_cast<uint8_t>(mask)) : (commonFlags & ~static_cast<uint8_t>(mask));
}

MessageHeader::SegmentType MessageHeader::getSegmentType() const
{
    return static_cast<SegmentType>((commonFlags & to_underlying(CommonFlags::seg)));
}

void MessageHeader::setSegmentType(const SegmentType type)
{
    commonFlags &= ~to_underlying(CommonFlags::seg);
    commonFlags |= to_underlying(type);
}

uint8_t MessageHeader::getPayloadType() const
{
    return payloadType;
}

void MessageHeader::setPayloadType(const uint8_t type)
{
    payloadType = type;
}

uint16_t MessageHeader::getPayloadLength() const
{
    return swapEndian(payloadLength);
}

void MessageHeader::setPayloadLength(const uint16_t length)
{
    payloadLength = swapEndian(length);
}

END_NAMESPACE_ASAM_CMP
