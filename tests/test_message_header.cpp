#include <gtest/gtest.h>

#include <asam_cmp/message_header.h>

using ASAM::CMP::MessageHeader;

class MessageHeaderFixture : public ::testing::Test
{
protected:
    template <typename T>
    using SetterFunc = void (MessageHeader::*)(T);

    template <typename T>
    using GetterFunc = T (MessageHeader::*)() const;

    template <typename T>
    bool TestSetterGetter(SetterFunc<T> setter, GetterFunc<T> getter, const T newValue)
    {
        (header.*setter)(newValue);
        T value = (header.*getter)();
        return (value == newValue);
    }

protected:
    MessageHeader header;
};

TEST_F(MessageHeaderFixture, HeaderSize)
{
    static_assert(sizeof(MessageHeader) == 16, "Size of the header according to the standard");
}

TEST_F(MessageHeaderFixture, Contructor)
{
    [[maybe_unused]] MessageHeader ch;
}

TEST_F(MessageHeaderFixture, Timestamp)
{
    ASSERT_TRUE(TestSetterGetter(&MessageHeader::setTimestamp, &MessageHeader::getTimestamp, uint64_t{55}));
}

TEST_F(MessageHeaderFixture, InterfaceId)
{
    ASSERT_TRUE(TestSetterGetter(&MessageHeader::setInterfaceId, &MessageHeader::getInterfaceId, uint32_t{55}));
}

TEST_F(MessageHeaderFixture, VendorId)
{
    ASSERT_TRUE(TestSetterGetter(&MessageHeader::setVendorId, &MessageHeader::getVendorId, uint16_t{55}));
}

TEST_F(MessageHeaderFixture, CommonFlags)
{
    ASSERT_TRUE(TestSetterGetter(&MessageHeader::setCommonFlags, &MessageHeader::getCommonFlags, uint8_t{55}));
}

TEST_F(MessageHeaderFixture, CommonFlag)
{
    constexpr MessageHeader::CommonFlags recalc = MessageHeader::CommonFlags::recalc;
    constexpr MessageHeader::CommonFlags diOnIf = MessageHeader::CommonFlags::diOnIf;
    constexpr MessageHeader::CommonFlags overflow = MessageHeader::CommonFlags::overflow;
    uint8_t newFlags = to_underlying(recalc) | to_underlying(diOnIf);

    header.setCommonFlags(newFlags);

    ASSERT_EQ(header.getCommonFlags(), newFlags);
    ASSERT_TRUE(header.getCommonFlag(recalc));
    ASSERT_TRUE(header.getCommonFlag(diOnIf));
    ASSERT_FALSE(header.getCommonFlag(overflow));

    header.setCommonFlag(overflow, true);
    header.setCommonFlag(recalc, false);
    newFlags = to_underlying(diOnIf) | to_underlying(overflow);

    ASSERT_EQ(header.getCommonFlags(), newFlags);
    ASSERT_FALSE(header.getCommonFlag(recalc));
    ASSERT_TRUE(header.getCommonFlag(diOnIf));
    ASSERT_TRUE(header.getCommonFlag(overflow));
}

TEST_F(MessageHeaderFixture, SegmentType)
{
    header.setCommonFlags(0);
    ASSERT_TRUE(TestSetterGetter(
        &MessageHeader::setSegmentType, &MessageHeader::getSegmentType, MessageHeader::SegmentType::lastSegment));

    ASSERT_FALSE(header.getCommonFlag(MessageHeader::CommonFlags::recalc));
    ASSERT_FALSE(header.getCommonFlag(MessageHeader::CommonFlags::insync));
    ASSERT_FALSE(header.getCommonFlag(MessageHeader::CommonFlags::diOnIf));
    ASSERT_FALSE(header.getCommonFlag(MessageHeader::CommonFlags::overflow));
    ASSERT_FALSE(header.getCommonFlag(MessageHeader::CommonFlags::errorInPayload));
}

TEST_F(MessageHeaderFixture, PayloadType)
{
    ASSERT_TRUE(TestSetterGetter(&MessageHeader::setPayloadType, &MessageHeader::getPayloadType, uint8_t{100}));
}

TEST_F(MessageHeaderFixture, PayloadLength)
{
    ASSERT_TRUE(TestSetterGetter(&MessageHeader::setPayloadLength, &MessageHeader::getPayloadLength, uint16_t{55}));
}
