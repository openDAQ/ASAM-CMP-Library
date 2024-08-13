#pragma once

#include <asam_cmp/common.h>

BEGIN_NAMESPACE_TECMP

#pragma pack(push, 1)

class CmpHeader
{
    uint8_t IsTecmp{0};
    uint8_t deviceId{0};
    uint16_t sequenceCounter{0};
    uint8_t version{0};
    uint8_t messageType{0xFF};
    uint16_t dataType{0xFF};
    uint16_t reserved{0};
    uint16_t deviceFlags{0};
    uint32_t interfaceId{0};
    uint64_t timestamp{0};
    uint16_t payloadLength{0};
    uint16_t dataFlags{0};

public:
    enum class MessageType : uint8_t
    {
        control = 0x00,
        cmStatus = 0x01,
        busStatus = 0x02,
        data = 0x03,
        configStatus = 0x04,
        replayData = 0x0A,
        invalid = 0xFF
    };

    enum class DataType : uint16_t
    {
        can = 0x02,
        canFd = 0x03,
        lin = 0x04,
        flexRay = 0x08,
        uartRs232 = 0x10,
        analog = 0x20,
        ethernet = 0x80,
        invalid = 0xFF
    };

private:
    static constexpr uint16_t eosMask = 0x0100;             // 0x0001 -> 0x0100
    static constexpr uint16_t sosMask = 0x0200;             // 0x0002 -> 0x0200
    static constexpr uint16_t spyMask = 0x0400;             // 0x0004 -> 0x0400
    static constexpr uint16_t multiFrameMask = 0x0800;      // 0x0008 -> 0x0800
    static constexpr uint16_t reservedMask = 0xF03C;        // 0x3FF0 -> 0xF03C
    static constexpr uint16_t deviceOverflowMask = 0x0080;  // 0x8000 -> 0x0080;

public:
    uint8_t getDeviceId() const;
    void setDeviceId(uint8_t newId);

    uint16_t getSequenceCounter() const;
    void setSequenceCounter(uint16_t newCounter);

    uint8_t getVersion() const;
    void setVersion(uint8_t newVersion);

    MessageType getMessageType() const;
    void setMessageType(MessageType newType);

    DataType getDataType() const;
    void setDataType(DataType newType);

    uint16_t getDeviceFlags() const;
    void setDeviceFlags(uint16_t newFlags);

    uint32_t getInterfaceId() const;
    void setInterfaceId(uint32_t newId);

    uint64_t getTimestamp() const;
    void setTimestamp(uint64_t newTimestamp);

    uint16_t getPayloadLength() const;
    void setPayloadLength(uint16_t newLength);

    bool isValid() const;
};

#pragma pack(pop)

END_NAMESPACE_TECMP