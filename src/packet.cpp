#include <asam_cmp/analog_payload.h>
#include <asam_cmp/can_fd_payload.h>
#include <asam_cmp/can_payload.h>
#include <asam_cmp/lin_payload.h>
#include <asam_cmp/capture_module_payload.h>
#include <asam_cmp/ethernet_payload.h>
#include <asam_cmp/interface_payload.h>
#include <asam_cmp/packet.h>
#include <asam_cmp/payload_type.h>
#include <stdexcept>

BEGIN_NAMESPACE_ASAM_CMP

Packet::Packet(const CmpHeader::MessageType msgType, const uint8_t* data, const size_t size)
{
#ifdef _DEBUG
    if (data == nullptr || size < sizeof(MessageHeader))
        throw std::invalid_argument("Not enough data");
#else
    size;
#endif  // _DEBUG

    auto header = reinterpret_cast<const MessageHeader*>(data);
    payload = create({msgType, header->getPayloadType()}, data + sizeof(MessageHeader), header->getPayloadLength());
}

Packet::Packet(const Packet& other)
    : payload(new Payload(*(other.payload.get())))
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

    if (lhs.getStreamId() != rhs.getStreamId())
        return false;

    if (lhs.getVersion() != rhs.getVersion())
        return false;

    if (lhs.getPayloadSize() == rhs.getPayloadSize() && lhs.getPayloadSize() > 0)
        return lhs.getPayload() == rhs.getPayload();
    else
        return lhs.getPayloadSize() == rhs.getPayloadSize();
}

bool operator!=(const Packet& lhs, const Packet& rhs) noexcept
{
    return !operator==(lhs, rhs);
}

bool Packet::isValid() const
{
    return payload ? payload->isValid() : false;
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
    return payload->getType().getMessageType();
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

Payload& Packet::getPayload()
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
    switch (type.getType())
    {
        case PayloadType::can:
            if (CanPayload::isValidPayload(data, size))
                return std::make_unique<CanPayload>(data, size);
            break;
        case PayloadType::canFd:
            if (CanFdPayload::isValidPayload(data, size))
                return std::make_unique<CanFdPayload>(data, size);
            break;
        case PayloadType::lin:
            if (LinPayload::isValidPayload(data, size))
                return std::make_unique<LinPayload>(data, size);
            break;
        case PayloadType::analog:
            if (AnalogPayload::isValidPayload(data, size))
                return std::make_unique<AnalogPayload>(data, size);
            break;
        case PayloadType::ethernet:
            if (EthernetPayload::isValidPayload(data, size))
                return std::make_unique<EthernetPayload>(data, size);
            break;
        case PayloadType::cmStatMsg:
            if (CaptureModulePayload::isValidPayload(data, size))
                return std::make_unique<CaptureModulePayload>(data, size);
            break;
        case PayloadType::ifStatMsg:
            if (CaptureModulePayload::isValidPayload(data, size))
                return std::make_unique<InterfacePayload>(data, size);
            break;
        default:
            return std::make_unique<Payload>(type, data, size);
    }
    // In case of payload is not valid
    return std::make_unique<Payload>(PayloadType::invalid, data, size);
}

void swap(Packet& lhs, Packet& rhs) noexcept
{
    using std::swap;
    swap(lhs.version, rhs.version);
    swap(lhs.streamId, rhs.streamId);
    swap(lhs.deviceId, rhs.deviceId);
    swap(lhs.payload, rhs.payload);
}

END_NAMESPACE_ASAM_CMP
