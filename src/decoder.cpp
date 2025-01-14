#include <cstring>

#include <asam_cmp/decoder.h>
#include <asam_cmp/tecmp_decoder.h>
#include <iomanip>
#include <iostream>
#include <sstream>

BEGIN_NAMESPACE_ASAM_CMP

std::vector<std::shared_ptr<Packet>> Decoder::decode(const void* data, const std::size_t size)
{
    if (data == nullptr)
        return {};
    if (size < sizeof(CmpHeader))
        return {};

    const auto dataPtr = reinterpret_cast<const uint8_t*>(data);
    if (*dataPtr == 0x00)
        return TECMP::Decoder::Decode(data, size);

    std::vector<std::shared_ptr<Packet>> packets;
    const auto header = reinterpret_cast<const CmpHeader*>(data);
    const auto deviceId = header->getDeviceId();
    const auto streamId = header->getStreamId();
    auto packetPtr = reinterpret_cast<const uint8_t*>(header + 1);
    int curSize = static_cast<int>(size - sizeof(CmpHeader));
    std::shared_ptr<Packet> packet;
    while (curSize > 0)
    {
        if (!Packet::isValidPacket(packetPtr, curSize))
        {
            segmentedPackets.erase({deviceId, streamId});
            break;
        }

        if (!isSegmentedPacket(packetPtr, curSize))
        {
            segmentedPackets.erase({deviceId, streamId});

            packet = std::make_shared<Packet>(header->getMessageType(), packetPtr, curSize);

            packet->setVersion(header->getVersion());
            packet->setDeviceId(deviceId);
            packet->setStreamId(streamId);

            packets.push_back(packet);
        }
        else
        {
            if (isFirstSegment(packetPtr, curSize))
            {
                SegmentedPacket segmentedPacket(
                    packetPtr, curSize, header->getVersion(), header->getMessageType(), header->getSequenceCounter());
                segmentedPackets[{deviceId, streamId}] = std::move(segmentedPacket);
            }
            else
            {
                if (!segmentedPackets[{deviceId, streamId}].addSegment(
                        packetPtr, curSize, header->getVersion(), header->getMessageType(), header->getSequenceCounter()))
                {
                    segmentedPackets.erase({deviceId, streamId});
                }
                else if (segmentedPackets[{deviceId, streamId}].isAssembled())
                {
                    packet = segmentedPackets[{deviceId, streamId}].getPacket();

                    packet->setDeviceId(deviceId);
                    packet->setStreamId(streamId);
                    packets.push_back(packet);

                    segmentedPackets.erase({deviceId, streamId});
                }
            }
            break;
        }

        const auto packetSize = packet->getPayloadLength() + sizeof(MessageHeader);
        packetPtr += packetSize;
        curSize -= static_cast<int>(packetSize);
    }

    return packets;
}

bool Decoder::isSegmentedPacket(const uint8_t* data, const size_t)
{
    return reinterpret_cast<const MessageHeader*>(data)->getSegmentType() != MessageHeader::SegmentType::unsegmented;
}

bool Decoder::isFirstSegment(const uint8_t* data, const size_t)
{
    return reinterpret_cast<const MessageHeader*>(data)->getSegmentType() == MessageHeader::SegmentType::firstSegment;
}

Decoder::SegmentedPacket::SegmentedPacket(
    const uint8_t* data, const size_t size, const uint8_t version, const CmpHeader::MessageType messageType, const uint16_t sequenceCounter)
    : segmentType(SegmentType::firstSegment)
    , curVersion(version)
    , curMessageType(messageType)
    , curSegment(sequenceCounter)
{
    payload.resize(size);
    memcpy(payload.data(), data, size);
}

bool Decoder::SegmentedPacket::addSegment(
    const uint8_t* data, const size_t size, const uint8_t version, const CmpHeader::MessageType messageType, const uint16_t sequenceCounter)
{
    if (curVersion != version || curMessageType != messageType || sequenceCounter != curSegment + 1)
        return false;

    auto header = reinterpret_cast<const MessageHeader*>(data);
    auto newPayloadSize = header->getPayloadLength();
    if (newPayloadSize > size - sizeof(MessageHeader))
        return false;

    const SegmentType type = header->getSegmentType();
    if (!isValidSegmentType(type))
        return false;

    const auto curPayloadSize = payload.size();
    payload.resize(curPayloadSize + newPayloadSize);
    memcpy(payload.data() + curPayloadSize, data + sizeof(MessageHeader), newPayloadSize);
    getHeader()->setPayloadLength(static_cast<uint16_t>(payload.size()) - sizeof(MessageHeader));

    ++curSegment;
    segmentType = type;

    return true;
}

bool Decoder::SegmentedPacket::isAssembled() const
{
    return segmentType == SegmentType::lastSegment;
}

std::shared_ptr<Packet> Decoder::SegmentedPacket::getPacket()
{
    auto packet = std::make_shared<Packet>(curMessageType, payload.data(), payload.size());
    packet->setVersion(curVersion);
    return packet;
}

MessageHeader* Decoder::SegmentedPacket::getHeader()
{
    return reinterpret_cast<MessageHeader*>(payload.data());
}

bool Decoder::SegmentedPacket::isValidSegmentType(SegmentType type) const
{
    switch (segmentType)
    {
        case SegmentType::unsegmented:
        case SegmentType::lastSegment:
            return type == SegmentType::unsegmented || type == SegmentType::firstSegment;
        case SegmentType::firstSegment:
        case SegmentType::intermediarySegment:
            return type == SegmentType::intermediarySegment || type == SegmentType::lastSegment;
    }
    return false;
}

END_NAMESPACE_ASAM_CMP
