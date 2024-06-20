#include <cstring>

#include <asam_cmp/encoder.h>
#include <algorithm>

BEGIN_NAMESPACE_ASAM_CMP


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

uint16_t Encoder::getDeviceId() const
{
    return deviceId;
}

uint8_t Encoder::getStreamId() const
{
    return streamId;
}

void Encoder::setMessageType(const Packet& packet){
    messageType = packet.getMessageType();
    addNewCMPFrame(packet);
}

std::vector<std::vector<uint8_t>> Encoder::getEncodedData()
{
    cmpFrames.back().resize(std::max(cmpFrames.back().size() - bytesLeft, minBytesPerMessage), 0);
    auto frames = std::move(cmpFrames);
    clearEncodingMetadata(false);
    return frames;
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
        setMessageType(packet);

    size_t currentPayloadPos = 0;

    bool isSegmented = checkIfSegmented(packet);
    int segmentInd = 0;

    while (currentPayloadPos < packet.getPayloadLength())
    {
        if (bytesLeft < sizeof(MessageHeader))
            addNewCMPFrame(packet);

        uint16_t bytesToAdd = static_cast<uint16_t>(
            std::min(static_cast<size_t>(bytesLeft - sizeof(MessageHeader)), packet.getPayloadLength() - currentPayloadPos));

        SegmentType isSegmentedFlag =
            buildSegmentationFlag(isSegmented, segmentInd, bytesToAdd, packet.getPayloadLength(), currentPayloadPos);
        addNewDataHeader(packet, bytesToAdd, isSegmentedFlag);

        auto& cmpFrame = cmpFrames.back();
        memcpy(&cmpFrame[cmpFrame.size() - bytesLeft], packet.getPayload().getRawPayload(), bytesToAdd);
        ++segmentInd;
        currentPayloadPos += bytesToAdd;
        bytesLeft -= bytesToAdd;

        if (isSegmentedFlag == SegmentType::lastSegment)
            addNewCMPFrame(packet);
    }

}

void Encoder::restart()
{
    sequenceCounter = 0;
}

void Encoder::createCmpFrameTemplate(const Packet& packet)
{
    cmpFrameTemplate.resize(maxBytesPerMessage, 0);
    packet.getRawCmpHeader(cmpFrameTemplate.data());

    auto cmpHeader = reinterpret_cast<CMP::CmpHeader*>(cmpFrameTemplate.data());
    cmpHeader->setDeviceId(deviceId);
    cmpHeader->setStreamId(streamId);
}

void Encoder::addNewCMPFrame(const Packet& packet)
{
    if (!cmpFrames.empty())
        cmpFrames.back().resize(std::max(cmpFrames.back().size() - bytesLeft, minBytesPerMessage), 0);

    if (cmpFrameTemplate.empty())
        createCmpFrameTemplate(packet);

    cmpFrames.push_back(cmpFrameTemplate);
    auto header = reinterpret_cast<CmpHeader*>(cmpFrames.back().data());
    header->setSequenceCounter(++sequenceCounter);
    bytesLeft = maxBytesPerMessage - sizeof(CmpHeader);
}

void Encoder::addNewDataHeader(const Packet& packet, uint16_t bytesToAdd, SegmentType segmentationFlag)
{
    auto& cmpFrame = cmpFrames.back();
    MessageHeader* header = reinterpret_cast<MessageHeader*>(&cmpFrame[cmpFrame.size() - bytesLeft]);
    packet.getRawMessageHeader(header);
    header->setPayloadLength(bytesToAdd);
    header->setSegmentType(segmentationFlag);

    bytesLeft -= sizeof(MessageHeader);
}

bool Encoder::checkIfSegmented(const Packet& packet)
{
    bool isSegmented = (!cmpFrames.empty() && bytesLeft < sizeof(MessageHeader) + packet.getPayloadLength());
    if (isSegmented)
    {
        addNewCMPFrame(packet);
        isSegmented = (!cmpFrames.empty() && bytesLeft < sizeof(MessageHeader) + packet.getPayloadLength());
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
    cmpFrames.clear();
    cmpFrameTemplate.clear();

    if (clearSequenceCounter)
        sequenceCounter = 0;
}

END_NAMESPACE_ASAM_CMP
