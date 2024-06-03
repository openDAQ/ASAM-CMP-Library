#include <asam_cmp/encoder.h>

#include <stdexcept>
#include <limits>

BEGIN_NAMESPACE_ASAM_CMP

class Encoder::EncoderImpl
{
public:
    EncoderImpl()
            : deviceId(0)
            , streamId(0)
            , minBytesPerMessage(0)
            , maxBytesPerMessage(0)
            , bytesLeft(0)
            , sequenceCounter(0)
            , messageType(ASAM::CMP::Packet::MessageType::Undefined)
        {
        }

    void init(const DataContext& dataContext)
    {
        clearCachedVariables();
        minBytesPerMessage = dataContext.minBytesPerMessage;
        maxBytesPerMessage = dataContext.maxBytesPerMessage;
    }

    void setDeviceId(uint16_t newDeviceId)
    {
        deviceId = newDeviceId;
        clearCachedVariables();
    }

    void setStreamId(uint8_t newStreamId)
    {
        streamId = newStreamId;
        clearCachedVariables();
    }

    void setMessageType(ASAM::CMP::Packet::MessageType type)
    {
        addNewCMPFrame();
        messageType = type;
    }

    ASAM::CMP::Packet::MessageType  getMessageType() const
    {
        return messageType;
    }

    uint16_t getSequenceCounter() const
    {
        return sequenceCounter;
    }

    uint16_t getDeviceId() const
    {
        return deviceId;
    }

    uint8_t getStreamId() const
    {
        return streamId;
    }

    size_t getMaxBytesPerMessage() const
    {
        return maxBytesPerMessage;
    }

    size_t getMinBytesPerMessage() const
    {
        return minBytesPerMessage;
    }

    std::vector<std::vector<uint8_t>> getEncodedData()
    {
        cmpFrames.back().resize(std::max(cmpFrames.back().size() - bytesLeft, minBytesPerMessage), 0);
        auto frames = std::move(cmpFrames);
        clearCachedVariables();
        return frames;
    }

    bool checkIfIsSegmented(const size_t payloadSize)
    {
        bool isSegmented = (!cmpFrames.empty() && bytesLeft < sizeof(DataMessageHeader) + payloadSize);
        if (isSegmented)
        {
            addNewCMPFrame();
            isSegmented = (!cmpFrames.empty() && bytesLeft < sizeof(DataMessageHeader) + payloadSize);
        }
        return isSegmented;
    }

    uint8_t buildegmentationFlag(bool isSegmented, int segmentInd, uint16_t bytesToAdd, size_t payloadSize, size_t currentPayloadPos) const
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

    void addPayload(uint32_t interfaceId, Payload::Type payloadType, const uint8_t* payloadData, const size_t payloadSize)
    {
        size_t currentPayloadPos = 0;
        bool isSegmented = checkIfIsSegmented(payloadSize);

        int segmentInd = 0;

        while (currentPayloadPos < payloadSize)
        {
            if (bytesLeft <= sizeof(DataMessageHeader))
                addNewCMPFrame();

            uint16_t bytesToAdd = static_cast<uint16_t>(
                std::min(static_cast<size_t>(bytesLeft - sizeof(DataMessageHeader)), payloadSize - currentPayloadPos));

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

private:

    void clearCachedVariables()
    {
        sequenceCounter = 0;
        cmpFrames.clear();
        bytesLeft = 0;
        messageType = ASAM::CMP::Packet::MessageType::Undefined;
        cmpFrames.clear();
    }

    void addNewCMPFrame()
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

    void addNewDataHeader(uint32_t interfaceId, Payload::Type payloadType, uint16_t bytesToAdd, uint8_t segmentationFlag)
    {
        auto& cmpFrame = cmpFrames.back();
        DataMessageHeader* header = reinterpret_cast<DataMessageHeader*>(&cmpFrame[cmpFrame.size() - bytesLeft]);
        header->interfaceId = swapEndian(interfaceId);
        header->payloadType = static_cast<uint8_t>(payloadType);
        header->payloadLength = swapEndian(bytesToAdd);
        header->flags |= segmentationFlag;

        bytesLeft -= sizeof(DataMessageHeader);
    }

private:   
    constexpr static uint8_t segmentationFlagUnsegmented{0x00};
    constexpr static uint8_t segmentationFlagFirstSegment{0x01};
    constexpr static uint8_t segmentationFlagIntermediarySegment{0x10};
    constexpr static uint8_t segmentationFlagLastSegment{0x11};

    size_t minBytesPerMessage;
    size_t maxBytesPerMessage;
    uint16_t deviceId;
    uint8_t streamId;

    size_t bytesLeft;
    uint16_t sequenceCounter;

    ASAM::CMP::Packet::MessageType messageType;
    std::vector<std::vector<uint8_t>> cmpFrames;

private:
#pragma pack(push, 1)

    struct CmpMessageHeader
    {
        uint8_t version{1};
        uint8_t reserved{0};
        uint16_t deviceId{0};
        uint8_t messageType{0};
        uint8_t streamId{0};
        uint16_t sequenceCounter{0};
    };

    struct DataMessageHeader
    {
        uint64_t timestamp{0};
        uint32_t interfaceId{0};
        uint8_t flags{0};
        uint8_t payloadType{0};
        uint16_t payloadLength{0};
    };

#pragma pack(pop)
};

Encoder::Encoder()
    : impl(std::make_unique<EncoderImpl>())
{
}

void Encoder::init(const DataContext& dataContext)
{
    impl->init(dataContext);
}

std::vector<std::vector<uint8_t>> Encoder::encode(const Packet& packet, const DataContext& dataContext)
{
    init(dataContext);
    putPacket(packet);
    return getEncodedData();
}

void Encoder::putPacket(const Packet& packet)
{
    if (impl->getMessageType() != packet.getMessageType())
        impl->setMessageType(packet.getMessageType());

    impl->addPayload(0, packet.getPayload().getType(), packet.getPayload().getRawPayload(), packet.getPayload().getSize());
}

std::vector<std::vector<uint8_t>> Encoder::getEncodedData()
{
    return std::move(impl->getEncodedData());
}

void Encoder::setDeviceId(uint16_t deviceId)
{
    impl->setDeviceId(deviceId);
}

void Encoder::setStreamId(uint8_t streamId)
{
    impl->setStreamId(streamId);
}

uint16_t Encoder::getDeviceId() const
{
    return impl->getDeviceId();
}

uint8_t Encoder::getStreamId() const
{
    return impl->getStreamId();
}

uint16_t Encoder::getSequenceCounter() const
{
    return impl->getSequenceCounter();
}


END_NAMESPACE_ASAM_CMP
