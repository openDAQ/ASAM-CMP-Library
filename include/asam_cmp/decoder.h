#pragma once

#include <map>
#include <memory>
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

    struct Endpoint
    {
        uint16_t deviceId{0};
        uint8_t streamId{0};

        bool const operator<(const Endpoint& rhs) const
        {
            return deviceId < rhs.deviceId || (deviceId == rhs.deviceId && streamId < rhs.streamId);
        }
    };

#pragma pack(pop)

public:
    std::vector<std::shared_ptr<Packet>> decode(const void* data, const std::size_t size);

    using SegmentedPackets = std::map<Endpoint, std::shared_ptr<Packet>>;
    // TODO replace std::map by std::unoredered_map, but we need to implement hash function for Endpoint
    // using SegmetedPackets = std::unordered_map<Endpoint, std::shared_ptr<Packet>>;

private:
    SegmentedPackets segmentedPackets;
};

END_NAMESPACE_ASAM_CMP
