#pragma once

#include <memory>
#include <vector>

#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class Packet final
{
public:
    enum class MessageType : uint8_t
    {
        Undefined = 0x00,
        Data = 0x01,
        Control = 0x02,
        Status = 0x03,
        Vendor = 0xFF
    };

    struct MessageHeader
    {
        uint64_t timestamp{0};
        union
        {
            uint32_t interfaceId{0};
            struct V
            {
                uint16_t reserved;
                uint16_t vendorId;
            };
        };
        uint8_t flags{0};
        uint8_t payloadType{0};
        uint16_t payloadLength{0};
    };

public:
    Packet(const uint8_t* data, const size_t size);

    bool isValid() const;

    uint8_t getVersion() const;
    void setVersion(const uint8_t value);
    uint16_t getDeviceId() const;
    void setDeviceId(const uint16_t value);
    uint8_t getStreamId() const;
    void setStreamId(const uint8_t value);

    MessageType getMessageType() const;

    void setPayload(const Payload& newPayload);

    const Payload& getPayload() const;

protected:
    std::unique_ptr<Payload> create(const Payload::Type type, const uint8_t* data, const size_t size);

private:
    constexpr static uint8_t errorInPayload = 0x40;

private:
    std::unique_ptr<Payload> payload;

    uint8_t version{};
    uint16_t deviceId{};
    uint8_t streamId{};
};

END_NAMESPACE_ASAM_CMP
