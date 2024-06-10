#include <asam_cmp/cmp_header.h>

BEGIN_NAMESPACE_ASAM_CMP

uint8_t CmpHeader::getVersion() const
{
    return version;
}

void CmpHeader::setVersion(const uint8_t newVersion)
{
    version = newVersion;
}

uint16_t CmpHeader::getDeviceId() const
{
    return swapEndian(deviceId);
}

void CmpHeader::setDeviceId(const uint16_t id)
{
    deviceId = swapEndian(id);
}

CmpHeader::MessageType CmpHeader::getMessageType() const
{
    return static_cast<CmpHeader::MessageType>(messageType);
}

void CmpHeader::setMessageType(const CmpHeader::MessageType type)
{
    messageType = to_underlying(type);
}

uint8_t CmpHeader::getStreamId() const
{
    return streamId;
}

void CmpHeader::setStreamId(const uint8_t id)
{
    streamId = id;
}

uint16_t CmpHeader::getSequenceCounter() const
{
    return swapEndian(sequenceCounter);
}

void CmpHeader::setSequenceCounter(const uint16_t counter)
{
    sequenceCounter = swapEndian(counter);
}

END_NAMESPACE_ASAM_CMP
