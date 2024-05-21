#include <asam_cmp/decoder.h>

#include <stdexcept>

BEGIN_NAMESPACE_ASAM_CMP

std::vector<std::shared_ptr<Packet>> Decoder::decode(const void* data, const std::size_t size)
{
    if (data == nullptr)
        return std::vector<std::shared_ptr<Packet>>();
    if (size < sizeof(CmpHeader))
        return std::vector<std::shared_ptr<Packet>>();

    auto header = reinterpret_cast<const CmpHeader*>(data);
    auto packetData = reinterpret_cast<const uint8_t*>(header + 1);
    auto packetSize = size - sizeof(CmpHeader);
    if (!Packet::isValidPacket(packetData, packetSize))
        return std::vector<std::shared_ptr<Packet>>();

    auto packet = std::make_shared<Packet>(packetData, packetSize);

    packet->setVersion(swapEndian(header->version));
    packet->setDeviceId(swapEndian(header->deviceId));
    packet->setStreamId(swapEndian(header->streamId));

    return std::vector<std::shared_ptr<Packet>>(1, packet);
}

END_NAMESPACE_ASAM_CMP
