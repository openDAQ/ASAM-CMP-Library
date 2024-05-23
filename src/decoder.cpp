#include <asam_cmp/decoder.h>

#include <stdexcept>

BEGIN_NAMESPACE_ASAM_CMP

std::vector<std::shared_ptr<Packet>> Decoder::decode(const void* data, const std::size_t size)
{
    if (data == nullptr)
        return std::vector<std::shared_ptr<Packet>>();
    if (size < sizeof(CmpHeader))
        return std::vector<std::shared_ptr<Packet>>();

    std::vector<std::shared_ptr<Packet>> packets;
    auto header = reinterpret_cast<const CmpHeader*>(data);
    auto packetPtr = reinterpret_cast<const uint8_t*>(header + 1);
    auto curSize = size - sizeof(CmpHeader);
    while (curSize > 0)
    {
        if (!Packet::isValidPacket(packetPtr, curSize))
            break;

        auto packet = std::make_shared<Packet>(packetPtr);

        packet->setVersion(swapEndian(header->version));
        packet->setDeviceId(swapEndian(header->deviceId));
        packet->setStreamId(swapEndian(header->streamId));

        auto packetSize = packet->getPayload().getSize() + sizeof(Packet::MessageHeader);
        packetPtr += packetSize;
        curSize -= packetSize;

        packets.push_back(packet);
    }

    return packets;
}

END_NAMESPACE_ASAM_CMP
