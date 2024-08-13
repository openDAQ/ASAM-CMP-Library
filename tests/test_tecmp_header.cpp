#include <gtest/gtest.h>

#include <asam_cmp/tecmp_header.h>

using TECMP::CmpHeader;

class TecmpHeaderFixture : public ::testing::Test
{
public:
    TecmpHeaderFixture()
    {
        std::vector<uint8_t> data = {0x00, 0x43, 0x05, 0x5e, 0x03, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0f, 0x00,
                                     0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x61, 0x17, 0x77, 0xf4, 0x90, 0x00, 0x0d};

        memcpy(&cmpHeader, data.data(), data.size());
    }

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

TEST_F(TecmpHeaderFixture, HeaderSize)
{
    static_assert(sizeof(CmpHeader) == 28, "Size of the header according to the standard");
}

TEST_F(TecmpHeaderFixture, Contructor)
{
    [[maybe_unused]] CmpHeader ch;
}

TEST_F(TecmpHeaderFixture, DeviceId)
{
    ASSERT_TRUE(TestSetterGetter(&CmpHeader::setDeviceId, &CmpHeader::getDeviceId, uint8_t{55}));
}

TEST_F(TecmpHeaderFixture, SequenceCounter)
{
    ASSERT_TRUE(TestSetterGetter(&CmpHeader::setSequenceCounter, &CmpHeader::getSequenceCounter, uint16_t{55}));
}

TEST_F(TecmpHeaderFixture, Version)
{
    ASSERT_TRUE(TestSetterGetter(&CmpHeader::setVersion, &CmpHeader::getVersion, uint8_t{55}));
}

TEST_F(TecmpHeaderFixture, MessageType)
{
    ASSERT_TRUE(TestSetterGetter(&CmpHeader::setMessageType, &CmpHeader::getMessageType, TECMP::CmpHeader::MessageType::cmStatus));
}

TEST_F(TecmpHeaderFixture, DataType)
{
    ASSERT_TRUE(TestSetterGetter(&CmpHeader::setDataType, &CmpHeader::getDataType, TECMP::CmpHeader::DataType::can));
}

TEST_F(TecmpHeaderFixture, DeviceFlags)
{
    ASSERT_TRUE(TestSetterGetter(&CmpHeader::setDeviceFlags, &CmpHeader::getDeviceFlags, uint16_t{55}));
}