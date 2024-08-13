#include <asam_cmp/can_fd_payload.h>
#include <asam_cmp/can_payload.h>
#include <asam_cmp/capture_module_payload.h>
#include <asam_cmp/interface_payload.h>
#include <asam_cmp/tecmp_decoder.h>
#include <gtest/gtest.h>

using TECMP::Decoder;

class TecmpDecoderFixture : public ::testing::Test
{
};

TEST_F(TecmpDecoderFixture, FailsWithASAMCMP)
{
    std::vector<uint8_t> data = {0x01, 0x00, 0x12, 0x34, 0x01, 0x00, 0x07, 0x3d, 0x17, 0xe0, 0x2e, 0x73, 0x25, 0x13, 0x05, 0xd3,
                                 0x00, 0x00, 0x00, 0x01, 0x10, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x07, 0x00, 0x01, 0x00, 0x10, 0x95, 0x00, 0x00};

    auto packets = Decoder::Decode(data.data(), data.size());
    ASSERT_TRUE(packets.empty());
}

TEST_F(TecmpDecoderFixture, DecodeCaptureModulePayload)
{
    std::vector<uint8_t> data = {0x00, 0x43, 0x05, 0x5c, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x0f, 0xff,
                                 0x02, 0x00, 0x00, 0x00, 0x61, 0x14, 0xb5, 0x3d, 0xe0, 0x00, 0x2e, 0x0f, 0x00, 0x0c, 0x01,
                                 0x04, 0x00, 0x00, 0x18, 0x00, 0x43, 0x01, 0x61, 0x16, 0xe1, 0x00, 0x14, 0x07, 0x0a, 0x03,
                                 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x60, 0xde, 0xb9, 0x5d, 0x59,
                                 0x15, 0x14, 0x22, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    auto tecmpPackets = Decoder::Decode(data.data(), data.size());
    ASSERT_FALSE(tecmpPackets.empty());
    ASSERT_EQ(tecmpPackets[0]->getMessageType(), ASAM::CMP::CmpHeader::MessageType::status);
    ASSERT_EQ(tecmpPackets[0]->getPayload().getType(), ASAM::CMP::PayloadType::cmStatMsg);
    ASSERT_EQ(tecmpPackets[0]->getTimestamp(), 0x0000006114b53de0);
    auto payload = tecmpPackets[0]->getPayload();
    auto cmPayload = reinterpret_cast<ASAM::CMP::CaptureModulePayload&>(payload);
    ASSERT_EQ(cmPayload.getSerialNumber(), "23140065");
    ASSERT_EQ(cmPayload.getSoftwareVersion(), "v20.7.10");
    ASSERT_EQ(cmPayload.getHardwareVersion(), "v3.3");
    ASSERT_EQ(tecmpPackets[0]->getDeviceId(), 0x0043);
}

TEST_F(TecmpDecoderFixture, DecodeInterfacePayload)
{
    std::vector<uint8_t> data = {
        0x00, 0x43, 0x05, 0x5d, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x61, 0x14, 0xce,
        0x05, 0x58, 0x00, 0x78, 0x00, 0x00, 0x0c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x43, 0x01, 0x61, 0x16, 0xe1, 0x00, 0x00, 0x00, 0x10,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    auto packets = Decoder::Decode(data.data(), data.size());
    ASSERT_FALSE(packets.empty());
    ASSERT_EQ(packets.size(), 9);
    auto id = 0x10;
    ASSERT_EQ(packets[0]->getInterfaceId(), id);
    for (std::size_t i = 1; i < packets.size(); i++)
    {
        id += 0x10;
        ASSERT_FALSE(packets[i]->getInterfaceId() == packets[i - 1]->getInterfaceId());
        ASSERT_EQ(packets[i]->getInterfaceId(), id);
    }
}

TEST_F(TecmpDecoderFixture, DecodeCanPayload)
{
    std::vector<uint8_t> data = {0x00, 0x43, 0x05, 0x60, 0x03, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
                                 0x20, 0x00, 0x00, 0x00, 0x61, 0x1d, 0x69, 0x0d, 0x08, 0x00, 0x0d, 0x00, 0x01, 0x00, 0x00,
                                 0x00, 0x00, 0x08, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    auto packets = Decoder::Decode(data.data(), data.size());

    ASSERT_FALSE(packets.empty());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_EQ(packets[0]->getInterfaceId(), 0x20);
    auto payload = reinterpret_cast<ASAM::CMP::CanPayload&>(packets[0]->getPayload());
    ASSERT_EQ(payload.getId(), 0x0);
    uint8_t canData = *payload.getData();
    ASSERT_EQ(canData, 123);
}

TEST_F(TecmpDecoderFixture, DecodeCanFdPayload)
{
    std::vector<uint8_t> data = {0x00, 0x43, 0xe5, 0x60, 0x03, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x20, 0x00,
                                 0x00, 0x00, 0x7e, 0x8a, 0xb6, 0xff, 0x28, 0x00, 0x15, 0x00, 0x11, 0x00, 0x00, 0x03, 0x21, 0x10, 0x1d,
                                 0x5a, 0x64, 0x3b, 0xdf, 0xc7, 0x5e, 0x40, 0x42, 0x60, 0xe5, 0xd0, 0x22, 0x15, 0x74, 0x40};

    auto packets = Decoder::Decode(data.data(), data.size());
    ASSERT_FALSE(packets.empty());
    ASSERT_EQ(packets.size(), 1);
    ASSERT_EQ(packets[0]->getInterfaceId(), 0x20);
    auto payload = reinterpret_cast<ASAM::CMP::CanFdPayload&>(packets[0]->getPayload());
    ASSERT_EQ(payload.getId(), 0x321);
    ASSERT_EQ(payload.getDataLength(), 16);
    auto data1 = *(reinterpret_cast<const double*>(payload.getData()));
    ASSERT_EQ(data1, 123.123);
    auto data2 = *(reinterpret_cast<const double*>(payload.getData() + sizeof(double)));
    ASSERT_EQ(data2, 321.321);
}
