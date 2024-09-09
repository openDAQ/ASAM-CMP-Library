#include <asam_cmp/common.h>
#include <asam_cmp/tecmp_header.h>

using ASAM::CMP::swapEndian;
using ASAM::CMP::to_underlying;

uint8_t TECMP::CmpHeader::getDeviceId() const
{
    return swapEndian(deviceId);
}

void TECMP::CmpHeader::setDeviceId(uint8_t newId)
{
    deviceId = swapEndian(newId);
}

uint16_t TECMP::CmpHeader::getSequenceCounter() const
{
    return swapEndian(sequenceCounter);
}

void TECMP::CmpHeader::setSequenceCounter(uint16_t newCounter)
{
    sequenceCounter = swapEndian(newCounter);
}

uint8_t TECMP::CmpHeader::getVersion() const
{
    return swapEndian(version);
}

void TECMP::CmpHeader::setVersion(uint8_t newVersion)
{
    version = swapEndian(newVersion);
}

TECMP::CmpHeader::MessageType TECMP::CmpHeader::getMessageType() const
{
    return static_cast<CmpHeader::MessageType>(swapEndian(messageType));
}

void TECMP::CmpHeader::setMessageType(TECMP::CmpHeader::MessageType newType)
{
    messageType = to_underlying(newType);
}

TECMP::CmpHeader::DataType TECMP::CmpHeader::getDataType() const
{
    return static_cast<CmpHeader::DataType>(swapEndian(dataType));
}

void TECMP::CmpHeader::setDataType(TECMP::CmpHeader::DataType newType)
{
    dataType = swapEndian(to_underlying(newType));
}

uint16_t TECMP::CmpHeader::getDeviceFlags() const
{
    return swapEndian(deviceFlags);
}

void TECMP::CmpHeader::setDeviceFlags(uint16_t newFlags)
{
    deviceFlags = swapEndian(newFlags);
}
uint32_t TECMP::CmpHeader::getInterfaceId() const
{
    return swapEndian(interfaceId);
}
void TECMP::CmpHeader::setInterfaceId(uint32_t newId)
{
    interfaceId = swapEndian(newId);
}
uint64_t TECMP::CmpHeader::getTimestamp() const
{
    return swapEndian(timestamp);
}
void TECMP::CmpHeader::setTimestamp(uint64_t newTimestamp)
{
    timestamp = swapEndian(newTimestamp);
}
uint16_t TECMP::CmpHeader::getPayloadLength() const
{
    return swapEndian(payloadLength);
}
void TECMP::CmpHeader::setPayloadLength(uint16_t newLength)
{
    payloadLength = swapEndian(newLength);
}
bool TECMP::CmpHeader::isValid() const
{
    return !(messageType == 0xFF || dataType == 0xFF);
}
