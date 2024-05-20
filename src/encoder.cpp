#include <asam_cmp/encoder.h>

#include <stdexcept>

BEGIN_NAMESPACE_ASAM_CMP

Encoder::Encoder(uint16_t deviceId, uint8_t streamId)
    : deviceId(deviceId)
    , streamId(streamId)
    , bytesLeft(0)
    , sequenceCounter(0)
{
}

bool Encoder::hasFrame() const
{
    return !cmpFrames.empty();
}

std::vector<uint8_t> Encoder::getCmpMessage()
{
    if (cmpFrames.empty())
        return {};

    auto message = cmpFrames.front();
    cmpFrames.pop();
    return message;
}

void Encoder::addPayload(uint32_t interfaceId, Payload::PayloadType payloadType, const void* payloadData, const size_t payloadSize)
{
    size_t currentPayloadPos = 0;
    bool isSegmented = (bytesLeft < sizeof(DataMessageHeader) + payloadSize);
    int segmentInd = 0;

    while (currentPayloadPos < payloadSize)
    {
        if (bytesLeft <= sizeof(DataMessageHeader))
            addNewCMPFrame();

        uint16_t bytesToAdd = static_cast<uint16_t>(std::min(static_cast<size_t>(bytesLeft - sizeof(DataMessageHeader)), payloadSize - currentPayloadPos));

        uint8_t segmentationFlag = segmentationFlagUnsegmented;
        if (isSegmented)
        {
            if (segmentInd == 0)
                segmentationFlag = segmentationFlagFirstSegment;
            else
                segmentationFlag = (currentPayloadPos + bytesToAdd == payloadSize ? segmentationFlagLastSegment : segmentationFlagIntermediarySegment);
        }
        addNewDataHeader(interfaceId, payloadType, bytesToAdd, segmentationFlag);

        auto cmpFrame = cmpFrames.back();
        memcpy(&cmpFrame[cmpFrame.size() - bytesToAdd], payloadData, bytesToAdd);
        ++segmentInd;
        currentPayloadPos += bytesToAdd;
    }
}

void Encoder::addNewCMPFrame()
{
    std::vector<uint8_t> rawOutput(bytesPerCMPMessage);
    CmpMessageHeader* header = reinterpret_cast<CmpMessageHeader*>(&rawOutput[0]);
    header->deviceId = swap_endian(deviceId);
    header->streamId = swap_endian(streamId);
    header->sequenceCounter = swap_endian(++sequenceCounter);
    bytesLeft = bytesPerCMPMessage - sizeof(CmpMessageHeader);

    cmpFrames.push(std::move(rawOutput));
}

void Encoder::addNewDataHeader(uint32_t interfaceId, Payload::PayloadType payloadType, uint16_t bytesToAdd, uint8_t segmentationFlag)
{
    auto cmpFrame = cmpFrames.back();
    DataMessageHeader* header = reinterpret_cast<DataMessageHeader*>(cmpFrame.size() - bytesLeft);
    header->interfaceId = swap_endian(interfaceId);
    header->payloadType = static_cast<uint8_t>(payloadType);
    header->payloadLength = swap_endian(bytesToAdd);
    header->flags |= segmentationFlag;

    bytesLeft -= sizeof(DataMessageHeader);
}

END_NAMESPACE_ASAM_CMP
