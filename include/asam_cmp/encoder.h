#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>

#include <asam_cmp/common.h>
#include <asam_cmp/packet.h>

BEGIN_NAMESPACE_ASAM_CMP

class Encoder final
{
public:
    Encoder(uint16_t deviceId, uint8_t streamId);

    bool hasFrame() const;
    std::vector<uint8_t> getCmpMessage();
    void addPayload(uint32_t interfaceId, Payload::PayloadType payloadType, const void* payloadData, const size_t payloadSize);

private:

    void addNewCMPFrame();
    void addNewDataHeader(uint32_t interfaceId, Payload::PayloadType payloadType, uint16_t bytesToAdd, uint8_t segmentationFlag);

private:
    /// the limit for one CMP message (CMP header + packets)
    constexpr static uint32_t bytesPerCMPMessage{1500};

    constexpr static uint8_t segmentationFlagUnsegmented{0x00};
    constexpr static uint8_t segmentationFlagFirstSegment{0x00};
    constexpr static uint8_t segmentationFlagIntermediarySegment{0x00};
    constexpr static uint8_t segmentationFlagLastSegment{0x00};

    const uint16_t deviceId;
    const uint8_t streamId;

    uint32_t bytesLeft;
    uint16_t sequenceCounter;

    std::queue<std::vector<uint8_t>> cmpFrames;

private:
#pragma pack(push, 1)

    struct CmpMessageHeader
    {
        uint8_t version{1};
        uint8_t reserved{0};
        uint16_t deviceId{0};
        uint8_t messageType{0};
        uint8_t streamId{0};
        uint16_t sequenceCounter{0};
    };

    struct DataMessageHeader
    {
        uint64_t timestamp{0};
        uint32_t interfaceId{0};
        uint8_t flags{0};
        uint8_t payloadType{0};
        uint16_t payloadLength{0};
    };

#pragma pack(pop)
};

END_NAMESPACE_ASAM_CMP
