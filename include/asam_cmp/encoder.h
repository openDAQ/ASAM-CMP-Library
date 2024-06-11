#pragma once

#include <iterator>
#include <memory>
#include <queue>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <asam_cmp/common.h>
#include <asam_cmp/packet.h>

BEGIN_NAMESPACE_ASAM_CMP

struct DataContext
{
    size_t minBytesPerMessage;
    size_t maxBytesPerMessage;
};

class Encoder final
{
private:
    using SegmentType = MessageHeader::SegmentType;

public:
    Encoder();

    template <typename ForwardIterator>
    std::vector<std::vector<uint8_t>> encode(
        ForwardIterator begin,
        ForwardIterator end,
        const DataContext& dataContext,
        std::enable_if_t<std::is_same<typename std::iterator_traits<ForwardIterator>::value_type, Packet>::value, int> = 0);

    template <typename ForwardPtrIterator>
    std::vector<std::vector<uint8_t>> encode(
        ForwardPtrIterator begin,
        ForwardPtrIterator end,
        const DataContext& dataContext,
        std::enable_if_t<std::is_same<typename std::iterator_traits<ForwardPtrIterator>::value_type, std::shared_ptr<Packet>>::value, int> = 0);

    std::vector<std::vector<uint8_t>> encode(const Packet& packet, const DataContext& dataContext);

    void setDeviceId(uint16_t deviceId);
    void setStreamId(uint8_t streamId);
    void restart();

    uint16_t getDeviceId() const;
    uint8_t getStreamId() const;
    uint16_t getSequenceCounter() const;

private:
    void init(const DataContext& bytesPerMessage);
    void putPacket(const Packet& packet);
    std::vector<std::vector<uint8_t>> getEncodedData();

    bool checkIfIsSegmented(const size_t payloadSize);
    SegmentType buildSegmentationFlag(
        bool isSegmented, int segmentInd, uint16_t bytesToAdd, size_t payloadSize, size_t currentPayloadPos) const;
    void clearEncodingMetadata(bool clearSequenceCounter = false);

    void setMessageType(CmpHeader::MessageType type);
    void addPayload(uint32_t interfaceId, PayloadType payloadType, const uint8_t* payloadData, const size_t payloadSize);
    void addNewCMPFrame();
    void addNewDataHeader(uint32_t interfaceId, PayloadType payloadType, uint16_t bytesToAdd, SegmentType segmentationFlag);

private:
    size_t minBytesPerMessage;
    size_t maxBytesPerMessage;
    uint16_t deviceId;
    uint8_t streamId;

    size_t bytesLeft;
    uint16_t sequenceCounter;

    CmpHeader::MessageType messageType;
    std::vector<std::vector<uint8_t>> cmpFrames;
};

template <typename ForwardIterator>
std::vector<std::vector<uint8_t>> Encoder::encode(
    ForwardIterator begin,
    ForwardIterator end,
    const DataContext& dataContext,
    std::enable_if_t<std::is_same<typename std::iterator_traits<ForwardIterator>::value_type, Packet>::value, int>)
{
    static_assert(std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<ForwardIterator>::iterator_category>::value,
                  "ForwardIterator must be a forward iterator.");

    init(dataContext);

    for (auto it = begin; it != end; ++it)
        putPacket(*it);

    return getEncodedData();
}

template <typename ForwardPtrIterator>
std::vector<std::vector<uint8_t>> Encoder::encode(
    ForwardPtrIterator begin,
    ForwardPtrIterator end,
    const DataContext& dataContext,
    std::enable_if_t<std::is_same<typename std::iterator_traits<ForwardPtrIterator>::value_type, std::shared_ptr<Packet>>::value, int>)
{
    static_assert(std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<ForwardPtrIterator>::iterator_category>::value,
                  "ForwardIterator must be a forward iterator.");

    init(dataContext);

    for (auto it = begin; it != end; ++it)
        putPacket(*(*it));

    return getEncodedData();
}

END_NAMESPACE_ASAM_CMP
