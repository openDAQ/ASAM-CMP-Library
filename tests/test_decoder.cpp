#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/decoder.h>

using Decoder = AsamCmp::Decoder;

class DecoderFixture : public ::testing::Test
{
public:
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

    struct CanDataMessageHeader
    {
        uint16_t flags{0};
        uint16_t reserved{0};
        uint32_t arbId{0};
        uint32_t crc{0};
        uint16_t error_pos{0};
        uint8_t dlc{0};
        uint8_t data_length{0};
    };

#pragma pack(pop)

protected:
    template <typename Header>
    std::vector<uint8_t> createMessage(Header&& header, const std::vector<uint8_t>& data)
    {
        size_t messageSize = sizeof(header) + data.size();
        std::vector<uint8_t> message(messageSize);
        memcpy(&message[0], &header, sizeof(header));
        std::copy(data.begin(), data.end(), std::next(message.begin(), sizeof(header)));

        return message;
    }

    std::vector<uint8_t> createCanDataMessage(uint32_t arbId, const std::vector<uint8_t>& data)
    {
        CanDataMessageHeader header;
        header.arbId = swap_endian(arbId);

        return createMessage(header, data);
    }

    std::vector<uint8_t> createDataMessage(uint8_t payloadType, std::vector<uint8_t> payload)
    {
        DataMessageHeader header;
        header.payloadType = swap_endian(payloadType);
        header.payloadLength = swap_endian(static_cast<uint16_t>(payload.size()));

        return createMessage(header, payload);
    }

    std::vector<uint8_t> createCmpMessage(uint16_t deviceId, uint8_t messageType, uint8_t streamId, std::vector<uint8_t> payload)
    {
        CmpMessageHeader header;
        header.deviceId = swap_endian(deviceId);
        header.messageType = swap_endian(messageType);
        header.streamId = swap_endian(streamId);

        return createMessage(header, payload);
    }
};

TEST_F(DecoderFixture, Construct)
{
    Decoder decoder;
    decoder;
}

TEST_F(DecoderFixture, DataNull)
{
    Decoder decoder;
    ASSERT_THROW(decoder.decode(nullptr, 0), std::invalid_argument);
}

TEST_F(DecoderFixture, Decode)
{
    constexpr uint8_t version = 1;
    constexpr size_t canDataSize = 8;
    constexpr uint32_t arbId = 33;
    constexpr uint8_t payloadTypeCan = 0x01;
    constexpr uint16_t deviceId = 3;
    constexpr uint8_t cmpMessageTypeData = 0x01;
    constexpr uint8_t streamId = 0x01;

    std::vector<uint8_t> data(canDataSize);
    std::iota(data.begin(), data.end(), 0);
    auto canMsg = createCanDataMessage(arbId, data);
    auto dataMsg = createDataMessage(payloadTypeCan, canMsg);
    auto cmpMsg = createCmpMessage(deviceId, cmpMessageTypeData, streamId, dataMsg);

    Decoder decoder;
    auto packets = decoder.decode(&cmpMsg[0], cmpMsg.size());
    ASSERT_EQ(packets.size(), 1);

    auto packet = packets[0];
    ASSERT_EQ(packet->getVersion(), version);
    ASSERT_EQ(packet->getDeviceId(), deviceId);
    ASSERT_EQ(packet->getStreamId(), streamId);
    ASSERT_EQ(static_cast<uint8_t>(packet->getMessageType()), cmpMessageTypeData);
}
