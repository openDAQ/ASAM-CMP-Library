#include <asam_cmp/can_payload.h>
#include <asam_cmp/packet.h>

BEGIN_NAMESPACE_ASAM_CMP

Packet::Packet(const uint8_t* data, const size_t size)
{
    auto header = reinterpret_cast<const MessageHeader*>(data);
    if (data != nullptr && size >= sizeof(MessageHeader) && swapEndian(header->payloadLength) <= (size - sizeof(MessageHeader)) &&
        ((header->flags & errorInPayload) == 0))
    {
        payload = create(static_cast<Payload::Type>(header->payloadType), data + sizeof(MessageHeader), size - sizeof(MessageHeader));
    }
    else
        payload = create(Payload::Type::invalid, nullptr, 0);
}

bool Packet::isValid() const
{
    return payload->getType() != Payload::Type::invalid;
}

uint8_t Packet::getVersion() const
{
    return version;
}

void Packet::setVersion(const uint8_t value)
{
    version = value;
}

uint16_t Packet::getDeviceId() const
{
    return deviceId;
}

void Packet::setDeviceId(const uint16_t value)
{
    deviceId = value;
}

uint8_t Packet::getStreamId() const
{
    return streamId;
}

void Packet::setStreamId(const uint8_t value)
{
    streamId = value;
}

Packet::MessageType Packet::getMessageType() const
{
    return MessageType::Data;
}

void Packet::setPayload(const Payload& newPayload)
{
    payload = std::make_unique<Payload>(newPayload);
}

const Payload& Packet::getPayload() const
{
    return *payload;
}

std::unique_ptr<Payload> Packet::create(const Payload::Type type, const uint8_t* data, const size_t size)
{
    switch (type)
    {
        case Payload::Type::can:
            return std::make_unique<CanPayload>(data, size);
        default:
            return std::make_unique<Payload>(Payload::Type::invalid, data, size);
    }
}

END_NAMESPACE_ASAM_CMP
