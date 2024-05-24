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
    std::vector<std::shared_ptr<Packet>> decode(const void* data, const std::size_t size);

private:
#pragma pack(push, 1)

    struct CmpHeader
    {
        uint8_t version{1};
        uint8_t reserved{0};
        uint16_t deviceId{0};
        uint8_t messageType{0};
        uint8_t streamId{0};
        uint16_t sequenceCounter{0};
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

    using SegmentedPackets = std::map<Endpoint, std::shared_ptr<Packet>>;
    // TODO replace std::map by std::unoredered_map, but we need to implement hash function for Endpoint
    // using SegmetedPackets = std::unordered_map<Endpoint, std::shared_ptr<Packet>>;

private:
    SegmentedPackets segmentedPackets;
};

END_NAMESPACE_ASAM_CMP
