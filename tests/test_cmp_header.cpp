#include <gtest/gtest.h>

#include <asam_cmp/cmp_header.h>

using ASAM::CMP::CmpHeader;

class CmpHeaderFixture : public ::testing::Test
{
protected:
    template <typename T>
    using SetterFunc = void (CmpHeader::*)(T);

    template <typename T>
    using GetterFunc = T (CmpHeader::*)() const;

    template <typename T>
    bool TestSetterGetter(SetterFunc<T> setter, GetterFunc<T> getter, const T newValue)
    {
        (cmpHeader.*setter)(newValue);
        T value = (cmpHeader.*getter)();
        return (value == newValue);
    }

protected:
    CmpHeader cmpHeader;
};

TEST_F(CmpHeaderFixture, HeaderSize)
{
    static_assert(sizeof(CmpHeader) == 8, "Size of the header according to the standard");
}

TEST_F(CmpHeaderFixture, Contructor)
{
    [[maybe_unused]] CmpHeader ch;
}

TEST_F(CmpHeaderFixture, Version)
{
    ASSERT_TRUE(TestSetterGetter(&CmpHeader::setVersion, &CmpHeader::getVersion, uint8_t{55}));
}

TEST_F(CmpHeaderFixture, DeviceId)
{
    ASSERT_TRUE(TestSetterGetter(&CmpHeader::setDeviceId, &CmpHeader::getDeviceId, uint16_t{55}));
}

TEST_F(CmpHeaderFixture, MessageType)
{
    ASSERT_TRUE(TestSetterGetter(&CmpHeader::setMessageType, &CmpHeader::getMessageType, CmpHeader::MessageType::vendor));
}

TEST_F(CmpHeaderFixture, StreamId)
{
    ASSERT_TRUE(TestSetterGetter(&CmpHeader::setStreamId, &CmpHeader::getStreamId, uint8_t{55}));
}

TEST_F(CmpHeaderFixture, SequenceCounter)
{
    ASSERT_TRUE(TestSetterGetter(&CmpHeader::setSequenceCounter, &CmpHeader::getSequenceCounter, uint16_t{55}));
}
