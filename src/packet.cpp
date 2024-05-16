#include <asam_cmp/packet.h>

BEGIN_NAMESPACE_ASAM_CMP

Packet::Packet(const void* data, size_t size)
    : packData(size)
{
    if (size)
        memcpy(&packData[0], data, size);
}

uint8_t Packet::getVersion() const
{
    return version;
}

void Packet::setVersion(uint8_t value)
{
    version = value;
}

uint16_t Packet::getDeviceId() const
{
    return deviceId;
}

void Packet::setDeviceId(uint16_t value)
{
    deviceId = value;
}

uint8_t Packet::getStreamId() const
{
    return streamId;
}

void Packet::setStreamId(uint8_t value)
{
    streamId = value;
}

Packet::MessageType Packet::getMessageType() const
{
    return messageType;
}

void Packet::setMessageType(MessageType value)
{
    messageType = value;
}

END_NAMESPACE_ASAM_CMP
