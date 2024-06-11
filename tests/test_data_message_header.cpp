#include <gtest/gtest.h>

#include <asam_cmp/data_message_header.h>

using ASAM::CMP::DataMessageHeader;

class DataMessageHeaderFixture : public ::testing::Test
{
protected:
    template <typename T>
    using SetterFunc = void (DataMessageHeader::*)(T);

    template <typename T>
    using GetterFunc = T (DataMessageHeader::*)() const;

    template <typename T>
    bool TestSetterGetter(SetterFunc<T> setter, GetterFunc<T> getter, const T newValue)
    {
        (header.*setter)(newValue);
        T value = (header.*getter)();
        return (value == newValue);
    }

protected:
    DataMessageHeader header;
};

TEST_F(DataMessageHeaderFixture, HeaderSize)
{
    static_assert(sizeof(DataMessageHeader) == 16, "Size of the header according to the standard");
}

TEST_F(DataMessageHeaderFixture, Contructor)
{
    DataMessageHeader ch;
}

TEST_F(DataMessageHeaderFixture, Timestamp)
{
    ASSERT_TRUE(TestSetterGetter(&DataMessageHeader::setTimestamp, &DataMessageHeader::getTimestamp, uint64_t{55}));
}

TEST_F(DataMessageHeaderFixture, InterfaceId)
{
    ASSERT_TRUE(TestSetterGetter(&DataMessageHeader::setInterfaceId, &DataMessageHeader::getInterfaceId, uint32_t{55}));
}

TEST_F(DataMessageHeaderFixture, VendorId)
{
    ASSERT_TRUE(TestSetterGetter(&DataMessageHeader::setVendorId, &DataMessageHeader::getVendorId, uint16_t{55}));
}

TEST_F(DataMessageHeaderFixture, CommonFlags)
{
    ASSERT_TRUE(TestSetterGetter(&DataMessageHeader::setCommonFlags, &DataMessageHeader::getCommonFlags, uint8_t{55}));
}

TEST_F(DataMessageHeaderFixture, CommonFlag)
{
    constexpr DataMessageHeader::CommonFlags recalc = DataMessageHeader::CommonFlags::recalc;
    constexpr DataMessageHeader::CommonFlags diOnIf = DataMessageHeader::CommonFlags::diOnIf;
    constexpr DataMessageHeader::CommonFlags overflow = DataMessageHeader::CommonFlags::overflow;
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

TEST_F(DataMessageHeaderFixture, SegmentType)
{
    header.setCommonFlags(0);
    ASSERT_TRUE(TestSetterGetter(
        &DataMessageHeader::setSegmentType, &DataMessageHeader::getSegmentType, DataMessageHeader::SegmentType::lastSegment));

    ASSERT_FALSE(header.getCommonFlag(DataMessageHeader::CommonFlags::recalc));
    ASSERT_FALSE(header.getCommonFlag(DataMessageHeader::CommonFlags::insync));
    ASSERT_FALSE(header.getCommonFlag(DataMessageHeader::CommonFlags::diOnIf));
    ASSERT_FALSE(header.getCommonFlag(DataMessageHeader::CommonFlags::overflow));
    ASSERT_FALSE(header.getCommonFlag(DataMessageHeader::CommonFlags::errorInPayload));
}

TEST_F(DataMessageHeaderFixture, PayloadType)
{
    ASSERT_TRUE(TestSetterGetter(
        &DataMessageHeader::setPayloadType, &DataMessageHeader::getPayloadType, DataMessageHeader::PayloadType::uartRs232));
}

TEST_F(DataMessageHeaderFixture, PayloadLength)
{
    ASSERT_TRUE(TestSetterGetter(&DataMessageHeader::setPayloadLength, &DataMessageHeader::getPayloadLength, uint16_t{55}));
}
