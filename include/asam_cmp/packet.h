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
        uint64_t timestamp{};
        union
        {
            uint32_t interfaceId{};
            struct V
            {
                uint16_t reserved;
                uint16_t vendorId;
            };
        };

        uint8_t commonFlags{};
        uint8_t payloadType{};
        uint16_t payloadLength{};

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

private:
    std::unique_ptr<Payload> create(const Payload::Type type, const uint8_t* data, const size_t size);
    bool isValidSegmentType(SegmentType type);

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
