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
    std::vector<std::shared_ptr<Packet>> decode(const void* data, const std::size_t size);

private:
    static bool isSegmentedPacket(const uint8_t* data, const size_t);
    static bool isFirstSegment(const uint8_t* data, const size_t);

private:
    struct Endpoint
    {
        uint16_t deviceId{0};
        uint8_t streamId{0};

        bool operator==(const Endpoint& rhs) const
        {
            return (deviceId == rhs.deviceId) && (streamId == rhs.streamId);
        }
    };

    struct EndpointHash
    {
        std::size_t operator()(const Endpoint& rhs) const
        {
            return (rhs.deviceId | (rhs.streamId << 16));
        }
    };

    class SegmentedPacket final
    {
        using SegmentType = MessageHeader::SegmentType;

    public:
        SegmentedPacket() = default;
        SegmentedPacket(const uint8_t* data,
                        const size_t size,
                        uint8_t version,
                        const CmpHeader::MessageType messageType,
                        const uint16_t sequenceCounter);

        bool addSegment(const uint8_t* data,
                        const size_t size,
                        const uint8_t version,
                        const CmpHeader::MessageType messageType,
                        const uint16_t sequenceCounter);

        bool isAssembled() const;
        std::shared_ptr<Packet> getPacket();

    private:
        MessageHeader* getHeader();
        bool isValidSegmentType(SegmentType type) const;

        std::vector<uint8_t> payload;
        SegmentType segmentType{SegmentType::unsegmented};
        uint8_t curVersion{0};
        CmpHeader::MessageType curMessageType{0};
        uint16_t curSegment{0};
    };

    using SegmentedPackets = std::unordered_map<Endpoint, SegmentedPacket, EndpointHash>;

private:
    SegmentedPackets segmentedPackets;
};

END_NAMESPACE_ASAM_CMP
