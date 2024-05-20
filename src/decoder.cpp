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
    auto packet = std::make_shared<Packet>(reinterpret_cast<const uint8_t*>(header + 1), size - sizeof(CmpHeader));

    if (packet->isValid())
    {
        packet->setVersion(swapEndian(header->version));
        packet->setDeviceId(swapEndian(header->deviceId));
        packet->setStreamId(swapEndian(header->streamId));
    }

    return std::vector<std::shared_ptr<Packet>>(1, packet);
}

END_NAMESPACE_ASAM_CMP
