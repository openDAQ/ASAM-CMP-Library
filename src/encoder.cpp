#include <asam_cmp/encoder.h>

#include <stdexcept>
#include <limits>

BEGIN_NAMESPACE_ASAM_CMP

Encoder::Encoder()
    : deviceId(0)
    , streamId(0)
    , minBytesPerMessage(0)
    , maxBytesPerMessage(0)
    , bytesLeft(0)
    , sequenceCounter(0)
    , messageType(ASAM::CMP::Packet::MessageType::Undefined)
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

void Encoder::setMessageType(ASAM::CMP::Packet::MessageType type)
{
    addNewCMPFrame();
    messageType = type;
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

void Encoder::addPayload(uint32_t interfaceId, Payload::Type payloadType, const uint8_t* payloadData, const size_t payloadSize)
{
    size_t currentPayloadPos = 0;
    bool isSegmented = checkIfIsSegmented(payloadSize);

    int segmentInd = 0;

    while (currentPayloadPos < payloadSize)
    {
        if (bytesLeft <= sizeof(DataMessageHeader))
            addNewCMPFrame();

        uint16_t bytesToAdd =
            static_cast<uint16_t>(std::min(static_cast<size_t>(bytesLeft - sizeof(DataMessageHeader)), payloadSize - currentPayloadPos));

        uint8_t isSegmentedFlag = buildegmentationFlag(isSegmented, segmentInd, bytesToAdd, payloadSize, currentPayloadPos);
        uint8_t flag = isSegmentedFlag;
        addNewDataHeader(interfaceId, payloadType, bytesToAdd, flag);

        auto& cmpFrame = cmpFrames.back();
        memcpy(&cmpFrame[cmpFrame.size() - bytesLeft], payloadData, bytesToAdd);
        ++segmentInd;
        currentPayloadPos += bytesToAdd;
        bytesLeft -= bytesToAdd;

        if (isSegmentedFlag == segmentationFlagLastSegment)
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
    CmpMessageHeader* header = reinterpret_cast<CmpMessageHeader*>(&rawOutput[0]);
    header->deviceId = swapEndian(deviceId);
    header->version = 1;
    header->streamId = swapEndian(streamId);
    header->sequenceCounter = swapEndian(++sequenceCounter);
    header->messageType = static_cast<uint8_t>(messageType);
    bytesLeft = maxBytesPerMessage - sizeof(CmpMessageHeader);

    cmpFrames.push_back(std::move(rawOutput));
}

void Encoder::addNewDataHeader(uint32_t interfaceId, Payload::Type payloadType, uint16_t bytesToAdd, uint8_t segmentationFlag)
{
    auto& cmpFrame = cmpFrames.back();
    DataMessageHeader* header = reinterpret_cast<DataMessageHeader*>(&cmpFrame[cmpFrame.size() - bytesLeft]);
    header->interfaceId = swapEndian(interfaceId);
    header->payloadType = static_cast<uint8_t>(payloadType);
    header->payloadLength = swapEndian(bytesToAdd);
    header->flags |= segmentationFlag;

    bytesLeft -= sizeof(DataMessageHeader);
}

bool Encoder::checkIfIsSegmented(const size_t payloadSize)
{
    bool isSegmented = (!cmpFrames.empty() && bytesLeft < sizeof(DataMessageHeader) + payloadSize);
    if (isSegmented)
    {
        addNewCMPFrame();
        isSegmented = (!cmpFrames.empty() && bytesLeft < sizeof(DataMessageHeader) + payloadSize);
    }
    return isSegmented;
}

uint8_t Encoder::buildegmentationFlag(bool isSegmented, int segmentInd, uint16_t bytesToAdd, size_t payloadSize, size_t currentPayloadPos) const
{
    uint8_t segmentationFlag = segmentationFlagUnsegmented;
    if (isSegmented)
    {
        if (segmentInd == 0)
            segmentationFlag = segmentationFlagFirstSegment;
        else
            segmentationFlag =
                (currentPayloadPos + bytesToAdd == payloadSize ? segmentationFlagLastSegment : segmentationFlagIntermediarySegment);
    }

    return segmentationFlag;
}

void Encoder::clearEncodingMetadata(bool clearSequenceCounter)
{
    bytesLeft = 0;
    messageType = ASAM::CMP::Packet::MessageType::Undefined;
    cmpFrames.clear();

    if (clearSequenceCounter)
        sequenceCounter = 0;
}


END_NAMESPACE_ASAM_CMP
