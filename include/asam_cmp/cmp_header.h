#pragma once

#include <cstdint>

#include <asam_cmp/common.h>

BEGIN_NAMESPACE_ASAM_CMP

#pragma pack(push, 1)

class CmpHeader
{
    uint8_t version{1};
    uint8_t reserved{0};
    uint16_t deviceId{0};
    uint8_t messageType{0};
    uint8_t streamId{0};
    uint16_t sequenceCounter{0};

public:
    enum class MessageType : uint8_t
    {
        undefined = 0x00,
        data = 0x01,
        control = 0x02,
        status = 0x03,
        vendor = 0xFF
    };

public:
    uint8_t getVersion() const;
    void setVersion(const uint8_t newVersion);
    uint16_t getDeviceId() const;
    void setDeviceId(const uint16_t id);
    MessageType getMessageType() const;
    void setMessageType(const MessageType type);
    uint8_t getStreamId() const;
    void setStreamId(const uint8_t id);
    uint16_t getSequenceCounter() const;
    void setSequenceCounter(const uint16_t counter);
};

#pragma pack(pop)

END_NAMESPACE_ASAM_CMP
