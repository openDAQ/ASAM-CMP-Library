#include <asam_cmp/can_payload.h>
#include <asam_cmp/ethernet_payload.h>
#include <asam_cmp/packet.h>
#include <stdexcept>

BEGIN_NAMESPACE_ASAM_CMP

Packet::Packet(const uint8_t* data, const size_t size)
{
#ifdef _DEBUG
    if (data == nullptr || size < sizeof(MessageHeader))
        throw std::invalid_argument("Not enough data");
#else
    size;
#endif  // _DEBUG

    auto header = reinterpret_cast<const MessageHeader*>(data);
    payload = create(static_cast<Payload::Type>(header->payloadType), data + sizeof(MessageHeader), swapEndian(header->payloadLength));
    if (payload->getType() != Payload::Type::invalid)
    {
        segmentType = header->commonFlagsFields.seg;
    }
}

bool Packet::addSegment(const uint8_t* data, const size_t size)
{
    auto header = reinterpret_cast<const MessageHeader*>(data);
    segmentType = header->commonFlagsFields.seg;

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
    return (size >= sizeof(MessageHeader) && swapEndian(header->payloadLength) <= (size - sizeof(MessageHeader)) &&
            ((header->commonFlags & errorInPayload) == 0));
}

bool Packet::isSegmentedPacket(const uint8_t* data, const size_t)
{
    return reinterpret_cast<const MessageHeader*>(data)->commonFlagsFields.seg != SegmentType::unsegmented;
}

bool Packet::isFirstSegment(const uint8_t* data, const size_t)
{
    return reinterpret_cast<const MessageHeader*>(data)->commonFlagsFields.seg == SegmentType::firstSegment;
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

END_NAMESPACE_ASAM_CMP
