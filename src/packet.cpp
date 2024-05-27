#include <asam_cmp/can_payload.h>
#include <asam_cmp/ethernet_payload.h>
#include <asam_cmp/packet.h>
#include <stdexcept>

BEGIN_NAMESPACE_ASAM_CMP

Packet::SegmentType Packet::MessageHeader::getSegmentType() const
{
    return commonFlags.seg;
}

void Packet::MessageHeader::setSegmentType(const SegmentType type)
{
    commonFlags.seg = type;
}

bool Packet::MessageHeader::isErrorInPayload() const
{
    return commonFlags.error_in_payload == 1;
}

void Packet::MessageHeader::setErrorInPayload(bool error)
{
    error ? commonFlags.error_in_payload = 1 : 0;
}

Payload::Type Packet::MessageHeader::getPayloadType() const
{
    return static_cast<Payload::Type>(payloadType);
}

void Packet::MessageHeader::setPayloadType(const Payload::Type type)
{
    payloadType = static_cast<uint8_t>(type);
}

uint16_t Packet::MessageHeader::getPayloadLength() const
{
    return swapEndian(payloadLength);
}

void Packet::MessageHeader::setPayloadLength(const uint16_t length)
{
    payloadLength = swapEndian(length);
}

Packet::Packet(const uint8_t* data, const size_t size)
{
#ifdef _DEBUG
    if (data == nullptr || size < sizeof(MessageHeader))
        throw std::invalid_argument("Not enough data");
#else
    size;
#endif  // _DEBUG

    auto header = reinterpret_cast<const MessageHeader*>(data);
    if (isValidSegmentType(header->getSegmentType()))
        payload = create(static_cast<Payload::Type>(header->getPayloadType()), data + sizeof(MessageHeader), header->getPayloadLength());
    if (payload && payload->getType() != Payload::Type::invalid)
    {
        segmentType = header->getSegmentType();
    }
}

bool Packet::addSegment(const uint8_t* data, const size_t size)
{
    auto header = reinterpret_cast<const MessageHeader*>(data);
    if (!isValidSegmentType(header->getSegmentType()))
        return false;

    segmentType = header->getSegmentType();
    ++sequenceCounter;

    return payload->addSegment(data + sizeof(MessageHeader), size - sizeof(MessageHeader));
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

uint16_t Packet::getSequenceCounter() const
{
    return sequenceCounter;
}

void Packet::setSequenceCounter(const uint16_t value)
{
    sequenceCounter = value;
}

Packet::MessageType Packet::getMessageType() const
{
    return MessageType::Data;
}

size_t Packet::getPayloadSize() const
{
    return payload ? payload->getSize() : 0;
}

Packet::SegmentType Packet::getSegmentType() const
{
    return segmentType;
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
    return (size >= sizeof(MessageHeader) && header->getPayloadLength() <= (size - sizeof(MessageHeader)) && !header->isErrorInPayload());
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
    switch (type)
    {
        case Payload::Type::can:
            if (CanPayload::isValidPayload(data, size))
                return std::make_unique<CanPayload>(data, size);
            else
                return std::make_unique<Payload>(Payload::Type::invalid, data, size);
        case Payload::Type::ethernet:
            return std::make_unique<EthernetPayload>(data, size);
        default:
            return std::make_unique<Payload>(Payload::Type::invalid, data, size);
    }
}

bool Packet::isValidSegmentType(SegmentType type)
{
    switch (segmentType)
    {
        case SegmentType::unsegmented:
        case SegmentType::lastSegment:
            return type == SegmentType::unsegmented || type == SegmentType::firstSegment;
        case SegmentType::firstSegment:
        case SegmentType::intermediarySegment:
            return type == SegmentType::intermediarySegment || type == SegmentType::lastSegment;
    }
    return false;
}

END_NAMESPACE_ASAM_CMP
