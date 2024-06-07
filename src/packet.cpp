#include <asam_cmp/can_payload.h>
#include <asam_cmp/capture_module_payload.h>
#include <asam_cmp/ethernet_payload.h>
#include <asam_cmp/analog_payload.h>
#include <asam_cmp/packet.h>
#include <stdexcept>

BEGIN_NAMESPACE_ASAM_CMP

uint8_t Packet::MessageHeader::getCommonFlags() const
{
    return commonFlags;
}

void Packet::MessageHeader::setCommonFlags(const uint8_t newFlags)
{
    commonFlags = newFlags;
}

bool Packet::MessageHeader::getCommonFlag(const CommonFlags mask) const
{
    return (commonFlags & static_cast<uint16_t>(mask)) != 0;
}

void Packet::MessageHeader::setCommonFlag(const CommonFlags mask, const bool value)
{
    commonFlags = value ? (commonFlags | static_cast<uint16_t>(mask)) : (commonFlags & ~static_cast<uint16_t>(mask));
}

Packet::SegmentType Packet::MessageHeader::getSegmentType() const
{
    return static_cast<SegmentType>((commonFlags & to_underlying(CommonFlags::seg)));
}

void Packet::MessageHeader::setSegmentType(const SegmentType type)
{
    commonFlags &= ~to_underlying(CommonFlags::seg);
    commonFlags |= to_underlying(type);
}

Payload::Type Packet::MessageHeader::getPayloadType() const
{
    return static_cast<Payload::Type>(payloadType);
}

void Packet::MessageHeader::setPayloadType(const Payload::Type type)
{
    payloadType = to_underlying(type);
}

uint16_t Packet::MessageHeader::getPayloadLength() const
{
    return swapEndian(payloadLength);
}

void Packet::MessageHeader::setPayloadLength(const uint16_t length)
{
    payloadLength = swapEndian(length);
}

Packet::Packet(const MessageType msgType, const uint8_t* data, const size_t size)
    : messageType(msgType)
{
#ifdef _DEBUG
    if (data == nullptr || size < sizeof(MessageHeader))
        throw std::invalid_argument("Not enough data");
#else
    size;
#endif  // _DEBUG

    auto header = reinterpret_cast<const MessageHeader*>(data);
    payload = create(static_cast<Payload::Type>(header->getPayloadType()), data + sizeof(MessageHeader), header->getPayloadLength());
}

Packet::Packet(const Packet& other)
    : messageType(other.messageType)
    , version(other.version)
    , deviceId(other.deviceId)
    , streamId(other.streamId)
    , payload(new Payload(*(other.payload.get())))
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

Packet::MessageType Packet::getMessageType() const
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
            !header->getCommonFlag(CommonFlags::errorInPayload));
}

bool Packet::isSegmentedPacket(const uint8_t* data, const size_t)
{
    return reinterpret_cast<const MessageHeader*>(data)->getSegmentType() != SegmentType::unsegmented;
}

bool Packet::isFirstSegment(const uint8_t* data, const size_t)
{
    return reinterpret_cast<const MessageHeader*>(data)->getSegmentType() == SegmentType::firstSegment;
}

std::unique_ptr<Payload> Packet::create(const Payload::Type type, const uint8_t* data, const size_t size)
{
    switch (messageType)
    {
        case MessageType::data:
            return createDataPayload(type, data, size);
            break;
        case MessageType::status:
            return createStatusPayload(type, data, size);
            break;
    }

    return std::make_unique<Payload>(Payload::Type::invalid, data, size);
}

std::unique_ptr<Payload> Packet::createDataPayload(const Payload::Type type, const uint8_t* data, const size_t size)
{
    switch (type)
    {
        case Payload::Type::can:
            if (CanPayload::isValidPayload(data, size))
                return std::make_unique<CanPayload>(data, size);
            else
                return std::make_unique<Payload>(Payload::Type::invalid, data, size);
        case Payload::Type::ethernet:
            if (EthernetPayload::isValidPayload(data, size))
                return std::make_unique<EthernetPayload>(data, size);
            else
                return std::make_unique<Payload>(Payload::Type::invalid, data, size);
        case Payload::Type::analog:
            if (AnalogPayload::isValidPayload(data, size))
                return std::make_unique<AnalogPayload>(data, size);
            else
                return std::make_unique<Payload>(Payload::Type::invalid, data, size);
        default:
            return std::make_unique<Payload>(Payload::Type::invalid, data, size);
    }

    return std::make_unique<Payload>(Payload::Type::invalid, data, size);
}

std::unique_ptr<Payload> Packet::createStatusPayload(const Payload::Type type, const uint8_t* data, const size_t size)
{
    switch (type)
    {
        case Payload::Type::cmStatMsg:
            if (CaptureModulePayload::isValidPayload(data, size))
                return std::make_unique<CaptureModulePayload>(data, size);
            break;
    }

    return std::make_unique<Payload>(Payload::Type::invalid, data, size);
}

END_NAMESPACE_ASAM_CMP
