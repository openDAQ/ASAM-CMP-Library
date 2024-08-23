#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/ethernet_payload.h>

#include "create_message.h"

using ASAM::CMP::EthernetPayload;
using ASAM::CMP::Payload;
using ASAM::CMP::PayloadType;

class EthernetPayloadTest : public ::testing::Test
{
public:
    EthernetPayloadTest()
    {
        data.resize(dataSize);
        std::iota(data.begin(), data.end(), uint8_t{});
        message = createEthernetDataMessage(data);
        payload = std::make_unique<EthernetPayload>(message.data(), message.size());
    }

protected:
    static constexpr size_t dataSize = 40;

protected:
    std::vector<uint8_t> data;
    std::vector<uint8_t> message;
    std::unique_ptr<EthernetPayload> payload;
    EthernetPayload dcPayload;
};

TEST_F(EthernetPayloadTest, HeaderSize)
{
    static_assert(sizeof(EthernetPayload::Header) == 6, "Size of the header according to the standard");
}

TEST_F(EthernetPayloadTest, DefaultConstructor)
{
    ASSERT_TRUE(dcPayload.isValid());
}

TEST_F(EthernetPayloadTest, Copy)
{
    auto payloadCopy(*payload);
    ASSERT_TRUE(*payload == payloadCopy);
}

TEST_F(EthernetPayloadTest, CopyAssignment)
{
    EthernetPayload payloadCopy;

    payloadCopy = *payload;
    ASSERT_TRUE(*payload == payloadCopy);
}

TEST_F(EthernetPayloadTest, Move)
{
    EthernetPayload payloadChecker(*payload);

    EthernetPayload payloadCopy(std::move(*payload));

    ASSERT_TRUE(payloadCopy == payloadChecker);
    ASSERT_FALSE(payloadCopy == *payload);
}

TEST_F(EthernetPayloadTest, MoveAssignment)
{
    EthernetPayload payloadChecker(*payload);

    EthernetPayload payloadCopy;
    payloadCopy = std::move(*payload);

    ASSERT_TRUE(payloadCopy == payloadChecker);
    ASSERT_FALSE(payloadCopy == *payload);
}

TEST_F(EthernetPayloadTest, Flags)
{
    constexpr EthernetPayload::Flags fcsErr = EthernetPayload::Flags::fcsErr;
    constexpr EthernetPayload::Flags txPortDown = EthernetPayload::Flags::txPortDown;
    constexpr EthernetPayload::Flags frameTooLongErr = EthernetPayload::Flags::frameTooLongErr;
    uint16_t newFlags = to_underlying(fcsErr) | to_underlying(txPortDown);

    payload->setFlags(newFlags);

    ASSERT_EQ(payload->getFlags(), newFlags);
    ASSERT_TRUE(payload->getFlag(fcsErr));
    ASSERT_TRUE(payload->getFlag(txPortDown));
    ASSERT_FALSE(payload->getFlag(frameTooLongErr));

    payload->setFlag(frameTooLongErr, true);
    payload->setFlag(fcsErr, false);
    newFlags = to_underlying(txPortDown) | to_underlying(frameTooLongErr);

    ASSERT_EQ(payload->getFlags(), newFlags);
    ASSERT_FALSE(payload->getFlag(fcsErr));
    ASSERT_TRUE(payload->getFlag(txPortDown));
    ASSERT_TRUE(payload->getFlag(frameTooLongErr));

    ASSERT_EQ(dcPayload.getFlags(), 0u);
}

TEST_F(EthernetPayloadTest, Properties)
{
    ASSERT_EQ(payload->getType(), PayloadType::ethernet);
    ASSERT_EQ(payload->getDataLength(), dataSize);
    ASSERT_TRUE(std::equal(data.begin(), data.end(), payload->getData()));

    ASSERT_EQ(dcPayload.getType(), PayloadType::ethernet);
    ASSERT_EQ(dcPayload.getDataLength(), 0u);
    ASSERT_EQ(dcPayload.getData(), nullptr);
}

TEST_F(EthernetPayloadTest, SetData)
{
    constexpr uint16_t newDataSize = dataSize * 2;
    std::vector<uint8_t> newData(newDataSize);
    std::iota(newData.begin(), newData.end(), uint8_t{});
    payload->setData(newData.data(), newDataSize);
    ASSERT_EQ(payload->getDataLength(), newDataSize);
    ASSERT_TRUE(std::equal(newData.begin(), newData.end(), payload->getData()));

    dcPayload.setData(newData.data(), newDataSize);
    ASSERT_TRUE(std::equal(newData.begin(), newData.end(), dcPayload.getData()));
}

TEST_F(EthernetPayloadTest, SetZeroData)
{
    payload->setData(nullptr, 0);
    ASSERT_EQ(payload->getDataLength(), 0);
    ASSERT_EQ(payload->getData(), nullptr);
}

TEST_F(EthernetPayloadTest, IsValidPayload)
{
    ASSERT_TRUE(EthernetPayload::isValidPayload(message.data(), message.size()));
}
