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
    size_t minBytesPerMessage{0};
    size_t maxBytesPerMessage{1500};
};

class Encoder final
{
private:
    using SegmentType = MessageHeader::SegmentType;

public:
    Encoder() = default;

    template <typename ForwardIterator,
              std::enable_if_t<std::is_same_v<typename std::iterator_traits<ForwardIterator>::value_type, Packet>, bool> = true>
    std::vector<std::vector<uint8_t>> encode(ForwardIterator begin, ForwardIterator end, const DataContext& dataContext);

    template <typename ForwardPtrIterator,
              std::enable_if_t<std::is_same_v<typename std::iterator_traits<ForwardPtrIterator>::value_type, std::shared_ptr<Packet>>,
                               bool> = true>
    std::vector<std::vector<uint8_t>> encode(ForwardPtrIterator begin, ForwardPtrIterator end, const DataContext& dataContext);

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

    bool checkIfSegmented(const Packet& packet);
    SegmentType buildSegmentationFlag(
        bool isSegmented, int segmentInd, uint16_t bytesToAdd, size_t payloadSize, size_t currentPayloadPos) const;
    void clearEncodingMetadata(bool clearSequenceCounter = false);

    void setMessageType(const Packet& packet);
    void addNewCMPFrame(const Packet& packet);
    void addNewDataHeader(const Packet& packet, uint16_t bytesToAdd, SegmentType segmentationFlag);

    void createCmpFrameTemplate(const Packet& packet);

private:
    size_t minBytesPerMessage{0};
    size_t maxBytesPerMessage{0};
    uint16_t deviceId{0};
    uint8_t streamId{0};
    std::vector<uint8_t> cmpFrameTemplate;

    size_t bytesLeft{0};
    uint16_t sequenceCounter{0};

    CmpHeader::MessageType messageType{CmpHeader::MessageType::undefined};
    std::vector<std::vector<uint8_t>> cmpFrames;
};

template <typename ForwardIterator,
          std::enable_if_t<std::is_same_v<typename std::iterator_traits<ForwardIterator>::value_type, Packet>, bool>>
std::vector<std::vector<uint8_t>> Encoder::encode(ForwardIterator begin, ForwardIterator end, const DataContext& dataContext)
{
    static_assert(std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<ForwardIterator>::iterator_category>::value,
                  "ForwardIterator must be a forward iterator.");

    init(dataContext);

    for (auto it = begin; it != end; ++it)
        putPacket(*it);

    return getEncodedData();
}

template <typename ForwardPtrIterator,
          std::enable_if_t<std::is_same_v<typename std::iterator_traits<ForwardPtrIterator>::value_type, std::shared_ptr<Packet>>, bool>>
std::vector<std::vector<uint8_t>> Encoder::encode(ForwardPtrIterator begin, ForwardPtrIterator end, const DataContext& dataContext)
{
    static_assert(std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<ForwardPtrIterator>::iterator_category>::value,
                  "ForwardIterator must be a forward iterator.");

    init(dataContext);

    for (auto it = begin; it != end; ++it)
        putPacket(*(*it));

    return getEncodedData();
}

END_NAMESPACE_ASAM_CMP
