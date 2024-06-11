#include <asam_cmp/can_payload.h>
#include <asam_cmp/can_fd_payload.h>
#include <asam_cmp/capture_module_payload.h>
#include <asam_cmp/ethernet_payload.h>
#include <asam_cmp/analog_payload.h>
#include <asam_cmp/packet.h>
#include <stdexcept>

BEGIN_NAMESPACE_ASAM_CMP

Packet::Packet(const CmpHeader::MessageType msgType, const uint8_t* data, const size_t size)
    : messageType(msgType)
{
#ifdef _DEBUG
    if (data == nullptr || size < sizeof(MessageHeader))
        throw std::invalid_argument("Not enough data");
#else
    size;
#endif  // _DEBUG

    auto header = reinterpret_cast<const MessageHeader*>(data);
    payload = create(static_cast<PayloadType>(header->getPayloadType()), data + sizeof(MessageHeader), header->getPayloadLength());
}

Packet::Packet(const Packet& other)
    : payload(new Payload(*(other.payload.get())))
    , messageType(other.messageType)
    , version(other.version)
    , deviceId(other.deviceId)
    , streamId(other.streamId)
{
}

Packet::Packet(Packet&& other) noexcept
{
    swap(*this, other);
}

Packet& Packet::operator=(const Packet& other)
{
    if (!(*this == other))
    {
        Packet tmp(other);
        swap(*this, tmp);
    }
    return *this;
}

Packet& Packet::operator=(Packet&& other) noexcept
{
    swap(*this, other);
    return *this;
}

bool operator==(const Packet& lhs, const Packet& rhs) noexcept
{
    if (lhs.getDeviceId() != rhs.getDeviceId())
        return false;

    if (lhs.getMessageType() != rhs.getMessageType())
        return false;

    if (lhs.getStreamId() != rhs.getStreamId())
        return false;

    if (lhs.getVersion() != rhs.getVersion())
        return false;

    if (lhs.getPayloadSize() == rhs.getPayloadSize() && lhs.getPayloadSize() > 0)
        return lhs.getPayload() == rhs.getPayload();
    else
        return lhs.getPayloadSize() == rhs.getPayloadSize();
}

void swap(Packet& lhs, Packet& rhs)
{
    using std::swap;
    swap(lhs.messageType, rhs.messageType);
    swap(lhs.version, rhs.version);
    swap(lhs.streamId, rhs.streamId);
    swap(lhs.deviceId, rhs.deviceId);
    swap(lhs.payload, rhs.payload);
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

CmpHeader::MessageType Packet::getMessageType() const
{
    return messageType;
}

size_t Packet::getPayloadSize() const
{
    return payload ? payload->getSize() : 0;
}

void Packet::setPayload(const Payload& newPayload)
{
    payload = std::make_unique<Payload>(newPayload);
}

const Payload& Packet::getPayload() const
{
    return *payload;
}

bool Packet::isValidPacket(const uint8_t* data, const size_t size)
{
    auto header = reinterpret_cast<const MessageHeader*>(data);
    return (size >= sizeof(MessageHeader) && header->getPayloadLength() <= (size - sizeof(MessageHeader)) &&
            !header->getCommonFlag(MessageHeader::CommonFlags::errorInPayload));
}

std::unique_ptr<Payload> Packet::create(const PayloadType type, const uint8_t* data, const size_t size)
{
    switch (messageType)
    {
        case CmpHeader::MessageType::data:
            return createDataPayload(type, data, size);
            break;
        case CmpHeader::MessageType::status:
            return createStatusPayload(type, data, size);
            break;
        default:
            break;
    }

    return std::make_unique<Payload>(PayloadType::invalid, data, size);
}

std::unique_ptr<Payload> Packet::createDataPayload(const PayloadType type, const uint8_t* data, const size_t size)
{
    switch (type)
    {
        case PayloadType::can:
            if (CanPayload::isValidPayload(data, size))
                return std::make_unique<CanPayload>(data, size);
            break;
        case PayloadType::canFd:
            if (CanFdPayload::isValidPayload(data, size))
                return std::make_unique<CanFdPayload>(data, size);
            break;
        case PayloadType::analog:
            if (AnalogPayload::isValidPayload(data, size))
                return std::make_unique<AnalogPayload>(data, size);
            break;
        case PayloadType::ethernet:
            if (EthernetPayload::isValidPayload(data, size))
                return std::make_unique<EthernetPayload>(data, size);
            break;
        default:
            return std::make_unique<Payload>(static_cast<PayloadType>(type), data, size);
    }
    // In case of payload is not valid
    return std::make_unique<Payload>(PayloadType::invalid, data, size);
}

std::unique_ptr<Payload> Packet::createStatusPayload(const PayloadType type, const uint8_t* data, const size_t size)
{
    switch (type)
    {
        case PayloadType::cmStatMsg:
            if (CaptureModulePayload::isValidPayload(data, size))
                return std::make_unique<CaptureModulePayload>(data, size);
            break;
        default:
            return std::make_unique<Payload>(static_cast<PayloadType>(type), data, size);
    }

    return std::make_unique<Payload>(PayloadType::invalid, data, size);
}

END_NAMESPACE_ASAM_CMP
