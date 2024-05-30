#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <iterator>
#include <type_traits>

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

private:
    void init(uint16_t deviceId, uint8_t streamId, const DataContext& bytesPerMessage);
    void putPacket(const Packet& packet);
    std::vector<std::vector<uint8_t>> getEncodedData();

private:
    static constexpr uint32_t defaultBytesPerMessage{1500};

    class EncoderImpl;
    std::shared_ptr<EncoderImpl> impl;
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

    // TODO: should be replaced by logger
    if (begin == end)
        throw std::invalid_argument("Packets range should not be empty");

    init(begin->getDeviceId(), begin->getStreamId(), dataContext);

        for (auto it = begin; it != end; ++it) putPacket(*it);

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

    // TODO: should be replaced by logger
    if (begin == end)
        throw std::invalid_argument("Packets range should not be empty");

    init((*begin)->getDeviceId(), (*begin)->getStreamId(), dataContext);

    for (auto it = begin; it != end; ++it)
        putPacket(*(*it));

    return getEncodedData();
}

END_NAMESPACE_ASAM_CMP
