#include <asam_cmp/decoder.h>

#include <stdexcept>

BEGIN_NAMESPACE_ASAM_CMP

uint8_t Decoder::CmpHeader::getVersion() const
{
    return version;
}

void Decoder::CmpHeader::setVersion(const uint8_t newVersion)
{
    version = newVersion;
}

uint16_t Decoder::CmpHeader::getDeviceId() const
{
    return swapEndian(deviceId);
}

void Decoder::CmpHeader::setDeviceId(const uint16_t id)
{
    deviceId = swapEndian(id);
}

uint8_t Decoder::CmpHeader::getMessageType() const
{
    return messageType;
}

void Decoder::CmpHeader::setMessageType(const uint8_t type)
{
    messageType = type;
}

uint8_t Decoder::CmpHeader::getStreamId() const
{
    return streamId;
}

void Decoder::CmpHeader::setStreamId(const uint8_t id)
{
    streamId = id;
}

uint16_t Decoder::CmpHeader::getSequenceCounter() const
{
    return swapEndian(sequenceCounter);
}

void Decoder::CmpHeader::setSequenceCounter(const uint16_t counter)
{
    sequenceCounter = swapEndian(counter);
}

std::vector<std::shared_ptr<Packet>> Decoder::decode(const void* data, const std::size_t size)
{
    if (data == nullptr)
        return std::vector<std::shared_ptr<Packet>>();
    if (size < sizeof(CmpHeader))
        return std::vector<std::shared_ptr<Packet>>();

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

        if (!Packet::isSegmentedPacket(packetPtr, curSize) || Packet::isFirstSegment(packetPtr, curSize))
        {
            packet = std::make_shared<Packet>(packetPtr, curSize);

            packet->setVersion(header->getVersion());
            packet->setDeviceId(deviceId);
            packet->setStreamId(streamId);

            if (packet->getSegmentType() == Packet::SegmentType::firstSegment)
            {
                segmentedPackets[{deviceId, streamId}] = std::move(packet);
                break;
            }
        }
        else
        {
            if (!segmentedPackets[{deviceId, streamId}]->addSegment(packetPtr, curSize))
            {
                segmentedPackets.erase({deviceId, streamId});
                break;
            }

            if (segmentedPackets[{deviceId, streamId}]->getSegmentType() == Packet::SegmentType::lastSegment)
            {
                packets.push_back(std::move(segmentedPackets[{deviceId, streamId}]));
                segmentedPackets.erase({deviceId, streamId});
            }
            break;
        }

        packets.push_back(packet);

        auto packetSize = packet->getPayloadSize() + sizeof(Packet::MessageHeader);
        packetPtr += packetSize;
        curSize -= static_cast<int>(packetSize);
    }

    return packets;
}

END_NAMESPACE_ASAM_CMP
