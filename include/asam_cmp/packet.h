#pragma once

#include <memory>
#include <vector>

#include <asam_cmp/common.h>

BEGIN_NAMESPACE_ASAM_CMP

class Packet
{
public:
    using Version = uint8_t;

    enum class MessageType : uint8_t
    {
        Undefined = 0x00,
        Data = 0x01,
        Control = 0x02,
        Status = 0x03,
        Vendor = 0xFF
    };

public:
    Packet(const void* data, size_t size);

    Version getVersion() const;
    void setVersion(Version value);
    uint8_t getDeviceId() const;
    void setDeviceId(uint8_t value);
    uint8_t getStreamId() const;
    void setStreamId(uint8_t value);

    // Message info
    MessageType getMessageType() const;
    void setMessageType(MessageType value);

private:
    std::vector<uint8_t> packData;

    Version version{};
    uint8_t deviceId{};
    uint8_t streamId{};
    MessageType messageType{MessageType::Undefined};
};

END_NAMESPACE_ASAM_CMP
