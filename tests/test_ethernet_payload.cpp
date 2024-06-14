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
    }

protected:
    static constexpr size_t dataSize = 40;

protected:
    std::vector<uint8_t> data;
    std::vector<uint8_t> message;
};

TEST_F(EthernetPayloadTest, HeaderSize)
{
    static_assert(sizeof(EthernetPayload::Header) == 6, "Size of the header according to the standard");
}

TEST_F(EthernetPayloadTest, DefaultConstructor)
{
    EthernetPayload pay;
    ASSERT_FALSE(pay.isValid());
}

TEST_F(EthernetPayloadTest, Properties)
{
    EthernetPayload payload(message.data(), message.size());

    ASSERT_EQ(payload.getType(), PayloadType::ethernet);
    ASSERT_EQ(payload.getDataLength(), dataSize);
    ASSERT_TRUE(std::equal(data.begin(), data.end(), payload.getData()));
}

TEST_F(EthernetPayloadTest, IsValidPayload)
{
    ASSERT_TRUE(EthernetPayload::isValidPayload(message.data(), message.size()));
}

TEST_F(EthernetPayloadTest, Flags)
{
    constexpr EthernetPayload::Flags fcsErr = EthernetPayload::Flags::fcsErr;
    constexpr EthernetPayload::Flags txPortDown = EthernetPayload::Flags::txPortDown;
    constexpr EthernetPayload::Flags frameTooLongErr = EthernetPayload::Flags::frameTooLongErr;
    uint16_t newFlags = to_underlying(fcsErr) | to_underlying(txPortDown);

    EthernetPayload payload(message.data(), message.size());
    payload.setFlags(newFlags);

    ASSERT_EQ(payload.getFlags(), newFlags);
    ASSERT_TRUE(payload.getFlag(fcsErr));
    ASSERT_TRUE(payload.getFlag(txPortDown));
    ASSERT_FALSE(payload.getFlag(frameTooLongErr));

    payload.setFlag(frameTooLongErr, true);
    payload.setFlag(fcsErr, false);
    newFlags = to_underlying(txPortDown) | to_underlying(frameTooLongErr);

    ASSERT_EQ(payload.getFlags(), newFlags);
    ASSERT_FALSE(payload.getFlag(fcsErr));
    ASSERT_TRUE(payload.getFlag(txPortDown));
    ASSERT_TRUE(payload.getFlag(frameTooLongErr));
}
