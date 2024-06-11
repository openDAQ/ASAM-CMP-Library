#include <asam_cmp/encoder.h>

BEGIN_NAMESPACE_ASAM_CMP

Encoder::Encoder()
    : minBytesPerMessage(0)
    , maxBytesPerMessage(0)
    , deviceId(0)
    , streamId(0)
    , bytesLeft(0)
    , sequenceCounter(0)
    , messageType(CmpHeader::MessageType::undefined)
{
}

void Encoder::setDeviceId(uint16_t newDeviceId)
{
    deviceId = newDeviceId;
    clearEncodingMetadata(true);
}

void Encoder::setStreamId(uint8_t newStreamId)
{
    streamId = newStreamId;
    clearEncodingMetadata(true);
}

void Encoder::setMessageType(CmpHeader::MessageType type)
{
    messageType = type;
    addNewCMPFrame();
}

std::vector<std::vector<uint8_t>> Encoder::getEncodedData()
{
    cmpFrames.back().resize(std::max(cmpFrames.back().size() - bytesLeft, minBytesPerMessage), 0);
    auto frames = std::move(cmpFrames);
    clearEncodingMetadata(false);
    return frames;
}

uint16_t Encoder::getDeviceId() const
{
    return deviceId;
}

uint8_t Encoder::getStreamId() const
{
    return streamId;
}

uint16_t Encoder::getSequenceCounter() const
{
    return sequenceCounter;
}

void Encoder::init(const DataContext& dataContext)
{
    clearEncodingMetadata(false);
    minBytesPerMessage = dataContext.minBytesPerMessage;
    maxBytesPerMessage = dataContext.maxBytesPerMessage;
}

std::vector<std::vector<uint8_t>> Encoder::encode(const Packet& packet, const DataContext& dataContext)
{
    init(dataContext);
    putPacket(packet);
    return getEncodedData();
}

void Encoder::putPacket(const Packet& packet)
{
    if (messageType != packet.getMessageType())
        setMessageType(packet.getMessageType());

    addPayload(0, packet.getPayload().getType(), packet.getPayload().getRawPayload(), packet.getPayload().getSize());
}

void Encoder::restart()
{
    sequenceCounter = 0;
}

void Encoder::addPayload(uint32_t interfaceId, PayloadType payloadType, const uint8_t* payloadData, const size_t payloadSize)
{
    size_t currentPayloadPos = 0;
    bool isSegmented = checkIfIsSegmented(payloadSize);

    int segmentInd = 0;

    while (currentPayloadPos < payloadSize)
    {
        if (bytesLeft <= sizeof(MessageHeader))
            addNewCMPFrame();

        uint16_t bytesToAdd =
            static_cast<uint16_t>(std::min(static_cast<size_t>(bytesLeft - sizeof(MessageHeader)), payloadSize - currentPayloadPos));

        SegmentType isSegmentedFlag = buildSegmentationFlag(isSegmented, segmentInd, bytesToAdd, payloadSize, currentPayloadPos);
        addNewDataHeader(interfaceId, payloadType, bytesToAdd, isSegmentedFlag);

        auto& cmpFrame = cmpFrames.back();
        memcpy(&cmpFrame[cmpFrame.size() - bytesLeft], payloadData, bytesToAdd);
        ++segmentInd;
        currentPayloadPos += bytesToAdd;
        bytesLeft -= bytesToAdd;

        if (isSegmentedFlag == SegmentType::lastSegment)
        {
            addNewCMPFrame();
        }
    }
}

void Encoder::addNewCMPFrame()
{
    if (!cmpFrames.empty())
        cmpFrames.back().resize(std::max(cmpFrames.back().size() - bytesLeft, minBytesPerMessage), 0);

    std::vector<uint8_t> rawOutput(maxBytesPerMessage);
    CmpHeader* header = reinterpret_cast<CmpHeader*>(&rawOutput[0]);
    header->setDeviceId(deviceId);
    header->setVersion(1);
    header->setStreamId(streamId);
    header->setSequenceCounter(++sequenceCounter);
    header->setMessageType(messageType);
    bytesLeft = maxBytesPerMessage - sizeof(CmpHeader);

    cmpFrames.push_back(std::move(rawOutput));
}

void Encoder::addNewDataHeader(uint32_t interfaceId, PayloadType payloadType, uint16_t bytesToAdd, SegmentType segmentationFlag)
{
    auto& cmpFrame = cmpFrames.back();
    MessageHeader* header = reinterpret_cast<MessageHeader*>(&cmpFrame[cmpFrame.size() - bytesLeft]);
    header->setInterfaceId(interfaceId);
    header->setPayloadType(payloadType);
    header->setPayloadLength(bytesToAdd);
    header->setSegmentType(segmentationFlag);

    bytesLeft -= sizeof(MessageHeader);
}

bool Encoder::checkIfIsSegmented(const size_t payloadSize)
{
    bool isSegmented = (!cmpFrames.empty() && bytesLeft < sizeof(MessageHeader) + payloadSize);
    if (isSegmented)
    {
        addNewCMPFrame();
        isSegmented = (!cmpFrames.empty() && bytesLeft < sizeof(MessageHeader) + payloadSize);
    }
    return isSegmented;
}

Encoder::SegmentType Encoder::buildSegmentationFlag(
    bool isSegmented, int segmentInd, uint16_t bytesToAdd, size_t payloadSize, size_t currentPayloadPos) const
{
    SegmentType segmentationFlag = SegmentType::unsegmented;
    if (isSegmented)
    {
        if (segmentInd == 0)
            segmentationFlag = SegmentType::firstSegment;
        else
            segmentationFlag =
                (currentPayloadPos + bytesToAdd == payloadSize ? SegmentType::lastSegment : SegmentType::intermediarySegment);
    }

    return segmentationFlag;
}

void Encoder::clearEncodingMetadata(bool clearSequenceCounter)
{
    bytesLeft = 0;
    messageType = CmpHeader::MessageType::undefined;
    cmpFrames.clear();

    if (clearSequenceCounter)
        sequenceCounter = 0;
}

END_NAMESPACE_ASAM_CMP
