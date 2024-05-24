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

    enum class SegmentType : uint8_t
    {
        unsegmented = 0,
        firstSegment = 1,
        intermediarySegment = 2,
        lastSegment = 3
    };

    class MessageHeader
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

        struct CommonFlags
        {
            uint8_t recalc : 1;
            uint8_t insync : 1;
            SegmentType seg : 2;
            uint8_t dir_on_if : 1;
            uint8_t overflow : 1;
            uint8_t error_in_payload : 1;
            uint8_t reserved : 1;
        } commonFlags;
        uint8_t payloadType{0};
        uint16_t payloadLength{0};

    public:
        SegmentType getSegmentType() const;
        void setSegmentType(const SegmentType type);
        bool isErrorInPayload() const;
        void setErrorInPayload(bool error);
        Payload::Type getPayloadType() const;
        void setPayloadType(const Payload::Type type);
        uint16_t getPayloadLength() const;
        void setPayloadLength(const uint16_t length);
    };

public:
    Packet(const uint8_t* data, const size_t size);

    bool addSegment(const uint8_t* data, const size_t size);

    uint8_t getVersion() const;
    void setVersion(const uint8_t value);
    uint16_t getDeviceId() const;
    void setDeviceId(const uint16_t value);
    uint8_t getStreamId() const;
    void setStreamId(const uint8_t value);
    uint16_t getSequenceCounter() const;
    void setSequenceCounter(const uint16_t value);

    MessageType getMessageType() const;
    size_t getPayloadSize() const;
    SegmentType getSegmentType() const;

    void setPayload(const Payload& newPayload);
    const Payload& getPayload() const;

    static bool isValidPacket(const uint8_t* data, const size_t size);
    static bool isSegmentedPacket(const uint8_t* data, const size_t);
    static bool isFirstSegment(const uint8_t* data, const size_t);

protected:
    std::unique_ptr<Payload> create(const Payload::Type type, const uint8_t* data, const size_t size);

private:
    constexpr static uint8_t errorInPayload = 0x40;

private:
    std::unique_ptr<Payload> payload;

    uint8_t version{};
    uint16_t deviceId{};
    uint8_t streamId{};
    uint16_t sequenceCounter{};
    SegmentType segmentType{};
};

END_NAMESPACE_ASAM_CMP
