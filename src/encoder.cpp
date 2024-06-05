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
        {
        }

    void init(uint16_t newDeviceId, uint8_t newStreamId, const DataContext& dataContext)
        {
            deviceId = newDeviceId;
            streamId = newStreamId;
            minBytesPerMessage = dataContext.minBytesPerMessage;
            maxBytesPerMessage = dataContext.maxBytesPerMessage;
            bytesLeft = 0;
            sequenceCounter = 0;
        }

        uint32_t getDeviceId() const
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
            return std::move(cmpFrames);
        }

        void addPayload(uint32_t interfaceId, Payload::Type payloadType, const uint8_t* payloadData, const size_t payloadSize)
        {
            size_t currentPayloadPos = 0;
            bool isSegmented = (!cmpFrames.empty() && bytesLeft < sizeof(DataMessageHeader) + payloadSize);
            int segmentInd = 0;

            while (currentPayloadPos < payloadSize)
            {
                if (bytesLeft <= sizeof(DataMessageHeader))
                    addNewCMPFrame();

                uint16_t bytesToAdd = static_cast<uint16_t>(
                    std::min(static_cast<size_t>(bytesLeft - sizeof(DataMessageHeader)), payloadSize - currentPayloadPos));

                uint8_t segmentationFlag = segmentationFlagUnsegmented;
                if (isSegmented)
                {
                    if (segmentInd == 0)
                        segmentationFlag = segmentationFlagFirstSegment;
                    else
                        segmentationFlag =
                            (currentPayloadPos + bytesToAdd == payloadSize ? segmentationFlagLastSegment : segmentationFlagIntermediarySegment);
                }
                addNewDataHeader(interfaceId, payloadType, bytesToAdd, segmentationFlag);

                auto& cmpFrame = cmpFrames.back();
                memcpy(&cmpFrame[cmpFrame.size() - bytesLeft], payloadData, bytesToAdd);
                ++segmentInd;
                currentPayloadPos += bytesToAdd;
                bytesLeft -= bytesToAdd;
            }
        }

private:

    void addNewCMPFrame()
    {
        std::vector<uint8_t> rawOutput(maxBytesPerMessage);
        CmpMessageHeader* header = reinterpret_cast<CmpMessageHeader*>(&rawOutput[0]);
        header->deviceId = swapEndian(deviceId);
        header->version = 1;
        header->streamId = swapEndian(streamId);
        header->sequenceCounter = swapEndian(++sequenceCounter);
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

void Encoder::init(uint16_t deviceId, uint8_t streamId, const DataContext& dataContext)
{
    impl->init(deviceId, streamId, dataContext);
}

void Encoder::putPacket(const Packet& packet)
{
    //TODO: should be replaced by logger
    if (packet.getDeviceId() != impl->getDeviceId())
        throw std::runtime_error("Provided device id doesn't match");

    // TODO: should be replaced by logger
    if (packet.getStreamId() != impl->getStreamId())
        throw std::runtime_error("Provided device id doesn't match");

    //TODO: should be removed once other types are implemented
    if (packet.getMessageType() != Packet::MessageType::data)
        throw std::runtime_error("Non-data message types are not implemented");

    switch (packet.getPayload().getType())
    {
        case Payload::Type::can:
            impl->addPayload(0, Payload::Type::can, packet.getPayload().getRawPayload(), packet.getPayload().getSize());
            break;
        default:
            //TODO: should be replaced by logger
            throw std::runtime_error("Provided Payload type is not supported");
    }
}

std::vector<std::vector<uint8_t>> Encoder::getEncodedData()
{
    return std::move(impl->getEncodedData());
}

END_NAMESPACE_ASAM_CMP
