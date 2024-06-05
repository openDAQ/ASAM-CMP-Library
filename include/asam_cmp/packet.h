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
        undefined = 0x00,
        data = 0x01,
        control = 0x02,
        status = 0x03,
        vendor = 0xFF
    };

    enum class SegmentType : uint8_t
    {
        unsegmented = 0,
        firstSegment = 0x04,
        intermediarySegment = 0x08,
        lastSegment = 0x0C
    };

    enum class CommonFlags : uint8_t
    {
        recalc = 0x01,
        insync = 0x02,
        seg = 0x0C,
        diOnIf = 0x10,
        overflow = 0x20,
        errorInPayload = 0x40
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

        uint8_t commonFlags{0};
        uint8_t payloadType{0};
        uint16_t payloadLength{0};

        static constexpr uint8_t segShift = 2;

    public:
        uint8_t getCommonFlags() const;
        void setCommonFlags(const uint8_t newFlags);
        bool getCommonFlag(const CommonFlags mask) const;
        void setCommonFlag(const CommonFlags mask, const bool value);

        SegmentType getSegmentType() const;
        void setSegmentType(const SegmentType type);
        Payload::Type getPayloadType() const;
        void setPayloadType(const Payload::Type type);
        uint16_t getPayloadLength() const;
        void setPayloadLength(const uint16_t length);
    };

public:
    Packet(const MessageType msgType, const uint8_t* data, const size_t size);

    uint8_t getVersion() const;
    void setVersion(const uint8_t value);
    uint16_t getDeviceId() const;
    void setDeviceId(const uint16_t value);
    uint8_t getStreamId() const;
    void setStreamId(const uint8_t value);

    MessageType getMessageType() const;
    size_t getPayloadSize() const;

    void setPayload(const Payload& newPayload);
    const Payload& getPayload() const;

    static bool isValidPacket(const uint8_t* data, const size_t size);
    static bool isSegmentedPacket(const uint8_t* data, const size_t);
    static bool isFirstSegment(const uint8_t* data, const size_t);

private:
    std::unique_ptr<Payload> create(const Payload::Type type, const uint8_t* data, const size_t size);
    std::unique_ptr<Payload> createDataPayload(const Payload::Type type, const uint8_t* data, const size_t size);
    std::unique_ptr<Payload> createStatusPayload(const Payload::Type type, const uint8_t* data, const size_t size);

private:
    constexpr static uint8_t errorInPayload = 0x40;

private:
    std::unique_ptr<Payload> payload;

    MessageType messageType{MessageType::undefined};
    uint8_t version{0};
    uint16_t deviceId{0};
    uint8_t streamId{0};
};

END_NAMESPACE_ASAM_CMP
