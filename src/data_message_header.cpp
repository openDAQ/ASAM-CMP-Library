#include <asam_cmp/data_message_header.h>


BEGIN_NAMESPACE_ASAM_CMP

uint64_t DataMessageHeader::getTimestamp() const
{
    return swapEndian(timestamp);
}

void DataMessageHeader::setTimestamp(const uint64_t newTimestamp)
{
    timestamp = swapEndian(newTimestamp);
}

uint32_t DataMessageHeader::getInterfaceId() const
{
    return swapEndian(interfaceId);
}

void DataMessageHeader::setInterfaceId(const uint32_t id)
{
    interfaceId = swapEndian(id);
}

uint16_t DataMessageHeader::getVendorId() const
{
    return swapEndian(statusVendor.vendorId);
}

void DataMessageHeader::setVendorId(const uint16_t id)
{
    statusVendor.vendorId = swapEndian(id);
}

uint8_t DataMessageHeader::getCommonFlags() const
{
    return commonFlags;
}

void DataMessageHeader::setCommonFlags(const uint8_t newFlags)
{
    commonFlags = newFlags;
}

bool DataMessageHeader::getCommonFlag(const CommonFlags mask) const
{
    return (commonFlags & static_cast<uint16_t>(mask)) != 0;
}

void DataMessageHeader::setCommonFlag(const CommonFlags mask, const bool value)
{
    commonFlags = value ? (commonFlags | static_cast<uint16_t>(mask)) : (commonFlags & ~static_cast<uint16_t>(mask));
}

DataMessageHeader::SegmentType DataMessageHeader::getSegmentType() const
{
    return static_cast<SegmentType>((commonFlags & to_underlying(CommonFlags::seg)));
}

void DataMessageHeader::setSegmentType(const SegmentType type)
{
    commonFlags &= ~to_underlying(CommonFlags::seg);
    commonFlags |= to_underlying(type);
}

DataMessageHeader::PayloadType DataMessageHeader::getPayloadType() const
{
    return static_cast<PayloadType>(payloadType);
}

void DataMessageHeader::setPayloadType(const PayloadType type)
{
    payloadType = to_underlying(type);
}

uint16_t DataMessageHeader::getPayloadLength() const
{
    return swapEndian(payloadLength);
}

void DataMessageHeader::setPayloadLength(const uint16_t length)
{
    payloadLength = swapEndian(length);
}

END_NAMESPACE_ASAM_CMP
