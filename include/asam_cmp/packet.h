#pragma once

#include <memory>
#include <vector>

#include <asam_cmp/cmp_header.h>
#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class Packet final
{
public:
    using PayloadType = MessageHeader::PayloadType;

public:
    Packet(const CmpHeader::MessageType msgType, const uint8_t* data, const size_t size);
    Packet(const Packet& other);
    Packet(Packet&& other) noexcept;

    Packet& operator=(const Packet& other);
    Packet& operator=(Packet&& other) noexcept;
    friend bool operator==(const Packet& lhs, const Packet& rhs) noexcept;

    uint8_t getVersion() const;
    void setVersion(const uint8_t value);
    uint16_t getDeviceId() const;
    void setDeviceId(const uint16_t value);
    uint8_t getStreamId() const;
    void setStreamId(const uint8_t value);

    CmpHeader::MessageType getMessageType() const;
    size_t getPayloadSize() const;

    void setPayload(const Payload& newPayload);
    const Payload& getPayload() const;

    static bool isValidPacket(const uint8_t* data, const size_t size);

private:
    std::unique_ptr<Payload> create(const PayloadType type, const uint8_t* data, const size_t size);
    std::unique_ptr<Payload> createDataPayload(const PayloadType type, const uint8_t* data, const size_t size);
    std::unique_ptr<Payload> createStatusPayload(const PayloadType type, const uint8_t* data, const size_t size);

    friend void swap(Packet& lhs, Packet& rhs);

private:
    constexpr static uint8_t errorInPayload = 0x40;

private:
    std::unique_ptr<Payload> payload;

    CmpHeader::MessageType messageType{CmpHeader::MessageType::undefined};
    uint8_t version{0};
    uint16_t deviceId{0};
    uint8_t streamId{0};
};

END_NAMESPACE_ASAM_CMP
