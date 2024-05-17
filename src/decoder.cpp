#include <asam_cmp/decoder.h>

#include <stdexcept>

BEGIN_NAMESPACE_ASAM_CMP

std::vector<std::shared_ptr<Packet>> Decoder::decode(const void* data, const std::size_t size)
{
    if (data == nullptr)
        return std::vector<std::shared_ptr<Packet>>();
    if (size < sizeof(CmpMessageHeader))
        return std::vector<std::shared_ptr<Packet>>();

    auto header = reinterpret_cast<const CmpMessageHeader*>(data);
    auto packet = std::make_shared<Packet>(reinterpret_cast<const uint8_t*>(header + 1), size - sizeof(CmpMessageHeader));

    packet->setVersion(swap_endian(header->version));
    packet->setDeviceId(swap_endian(header->deviceId));
    packet->setStreamId(swap_endian(header->streamId));
    packet->setMessageType(static_cast<Packet::MessageType>(swap_endian(header->messageType)));

    return std::vector<std::shared_ptr<Packet>>(1, packet);
}

END_NAMESPACE_ASAM_CMP
