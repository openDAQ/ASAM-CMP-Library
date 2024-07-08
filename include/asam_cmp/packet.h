#pragma once

#include <memory>
#include <vector>

#include <asam_cmp/cmp_header.h>
#include <asam_cmp/common.h>
#include <asam_cmp/payload.h>

BEGIN_NAMESPACE_ASAM_CMP

class Packet final
{
private:
    using MessageType = ASAM::CMP::CmpHeader::MessageType;
    using SegmentType = ASAM::CMP::MessageHeader::SegmentType;
    using CommonFlags = ASAM::CMP::MessageHeader::CommonFlags;

public:
    Packet() = default;
    Packet(const CmpHeader::MessageType msgType, const uint8_t* data, const size_t size);
    Packet(const Packet& other);
    Packet(Packet&& other) noexcept;

    Packet& operator=(const Packet& other);
    Packet& operator=(Packet&& other) noexcept;
    friend bool operator==(const Packet& lhs, const Packet& rhs) noexcept;
    friend bool operator!=(const Packet& lhs, const Packet& rhs) noexcept;

public:
    bool isValid() const;

    // CMP Header:
    uint8_t getVersion() const;
    void setVersion(const uint8_t value);
    uint16_t getDeviceId() const;
    void setDeviceId(const uint16_t value);
    MessageType getMessageType() const;
    uint8_t getStreamId() const;
    void setStreamId(const uint8_t value);
    uint16_t getSequenceCounter() const;
    void setSequenceCounter(uint16_t counter);
    void getRawCmpHeader(void* dest) const;

    // MessageHeader fields
    uint64_t getTimestamp() const;
    void setTimestamp(const uint64_t newTimestamp);
    uint32_t getInterfaceId() const;
    void setInterfaceId(const uint32_t id);
    uint16_t getVendorId() const;
    void setVendorId(const uint16_t id);
    uint8_t getCommonFlags() const;
    void setCommonFlags(const uint8_t flags);
    bool getCommonFlag(const CommonFlags mask) const;
    void setCommonFlag(const CommonFlags mask, const bool value);
    SegmentType getSegmentType() const;
    void setSegmentType(const SegmentType type);
    uint8_t getPayloadType() const;
    uint16_t getPayloadLength() const;
    void getRawMessageHeader(void* dest) const;

    void setPayload(const Payload& newPayload);
    const Payload& getPayload() const;
    Payload& getPayload();

    static bool isValidPacket(const uint8_t* data, const size_t size);

private:
    std::unique_ptr<Payload> create(const PayloadType type, const uint8_t* data, const size_t size);

    friend void swap(Packet& lhs, Packet& rhs) noexcept;
    void setMessageHeader(MessageHeader messageHeader);

private:
    constexpr static uint8_t errorInPayload = 0x40;

private:
    std::unique_ptr<Payload> payload;

    uint8_t version{0};
    uint16_t deviceId{0};
    uint8_t streamId{0};
    uint16_t sequenceCounter{0};

    uint64_t timestamp{0};
    uint32_t interfaceId{0};
    uint16_t vendorId{0};
    uint8_t commonFlags{0};
    SegmentType segmentType{SegmentType::unsegmented};
};

END_NAMESPACE_ASAM_CMP
