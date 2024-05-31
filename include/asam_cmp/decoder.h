#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <asam_cmp/common.h>
#include <asam_cmp/packet.h>

BEGIN_NAMESPACE_ASAM_CMP

class Decoder final
{
public:
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
        uint8_t getVersion() const;
        void setVersion(const uint8_t newVersion);
        uint16_t getDeviceId() const;
        void setDeviceId(const uint16_t id);
        uint8_t getMessageType() const;
        void setMessageType(const uint8_t type);
        uint8_t getStreamId() const;
        void setStreamId(const uint8_t id);
        uint16_t getSequenceCounter() const;
        void setSequenceCounter(const uint16_t counter);
    };

#pragma pack(pop)

public:
    std::vector<std::shared_ptr<Packet>> decode(const void* data, const std::size_t size);

private:
    struct Endpoint
    {
        uint16_t deviceId{0};
        uint8_t streamId{0};

        bool const operator==(const Endpoint& rhs) const
        {
            return (deviceId == rhs.deviceId) && (streamId == rhs.streamId);
        }
    };

    struct EndpointHash
    {
        std::size_t operator()(const Endpoint& rhs) const
        {
            return std::hash<uint32_t>()(rhs.deviceId | (rhs.streamId << 16));
        }
    };

    class SegmentedPacket final
    {
        using SegmentType = Packet::SegmentType;
        using MessageHeader = Packet::MessageHeader;

    public:
        SegmentedPacket() = default;
        SegmentedPacket(const uint8_t* data, const size_t size, uint8_t version, int8_t messageType, uint16_t sequenceCounter);

        bool addSegment(const uint8_t* data, const size_t size, uint8_t version, int8_t messageType, uint16_t sequenceCounter);

        bool isAssembled() const;
        std::shared_ptr<Packet> getPacket();

    private:
        MessageHeader* getHeader();
        bool isValidSegmentType(SegmentType type) const;

        std::vector<uint8_t> payload;
        SegmentType segmentType{SegmentType::unsegmented};
        uint8_t curVersion{0};
        uint8_t curMessageType{0};
        uint16_t curSegment{0};
    };

    using SegmentedPackets = std::unordered_map<Endpoint, SegmentedPacket, EndpointHash>;

private:
    SegmentedPackets segmentedPackets;
};

END_NAMESPACE_ASAM_CMP
