#include <asam_cmp/decoder.h>

#include <stdexcept>

BEGIN_NAMESPACE_ASAM_CMP

std::vector<std::shared_ptr<Packet>> Decoder::decode(const void* data, std::size_t size)
{
    if (data == nullptr)
        throw std::invalid_argument("Data can't be null");
    if (size < sizeof(CmpMessageHeader))
        throw std::invalid_argument("Not enough data");

    auto header = reinterpret_cast<const CmpMessageHeader*>(data);
    auto packet = std::make_shared<Packet>(header + 1, size - sizeof(CmpMessageHeader));
    packet->setVersion(swap_endian(header->version));
    packet->setDeviceId(swap_endian(header->deviceId));
    packet->setStreamId(swap_endian(header->streamId));
    packet->setMessageType(static_cast<Packet::MessageType>(swap_endian(header->messageType)));

    return std::vector<std::shared_ptr<Packet>>(1, packet);
}

END_NAMESPACE_ASAM_CMP
