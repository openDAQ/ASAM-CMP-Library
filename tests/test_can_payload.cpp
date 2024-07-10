#include <gtest/gtest.h>
#include <numeric>

#include <asam_cmp/can_fd_payload.h>
#include <asam_cmp/can_payload.h>
#include <asam_cmp/message_header.h>

#include "create_message.h"

using ASAM::CMP::CanFdPayload;
using ASAM::CMP::CanPayload;
using ASAM::CMP::CanPayloadBase;
using ASAM::CMP::Payload;
using ASAM::CMP::PayloadType;
using MessageType = ASAM::CMP::CmpHeader::MessageType;

class CanPayloadTest : public ::testing::Test
{
public:
    CanPayloadTest()
    {
        data.resize(canDataSize);
        std::iota(data.begin(), data.end(), uint8_t{});
        auto message = createCanDataMessage(arbId, data);

        canPayload = std::make_unique<CanPayload>(message.data(), message.size());
        canFdPayload = std::make_unique<CanFdPayload>(message.data(), message.size());
    }

protected:
    static constexpr size_t canDataSize = 64;
    static constexpr uint32_t arbId = 78;

protected:
    std::vector<uint8_t> data;
    std::unique_ptr<CanPayload> canPayload;
    std::unique_ptr<CanFdPayload> canFdPayload;

    CanPayload dcCanPayload;
    CanFdPayload dcCanFdPayload;
};

TEST_F(CanPayloadTest, HeaderSize)
{
    static_assert(sizeof(CanPayloadBase::Header) == 16, "Size of the header according to the standard");
}

TEST_F(CanPayloadTest, DefaultConstructorCan)
{
    ASSERT_TRUE(dcCanPayload.isValid());
}

TEST_F(CanPayloadTest, DefaultConstructorCanFd)
{
    ASSERT_TRUE(dcCanFdPayload.isValid());
}

TEST_F(CanPayloadTest, TypeCan)
{
    CanPayloadBase* payload = canPayload.get();
    ASSERT_EQ(payload->getType(), PayloadType::can);
    ASSERT_EQ(dcCanPayload.getType(), PayloadType::can);
}

TEST_F(CanPayloadTest, TypeCanFd)
{
    CanPayloadBase* payload = canFdPayload.get();
    ASSERT_EQ(payload->getType(), PayloadType::canFd);
    ASSERT_EQ(dcCanFdPayload.getType(), PayloadType::canFd);
}

TEST_F(CanPayloadTest, Flags)
{
    constexpr CanPayloadBase::Flags passiveAckErr = CanPayloadBase::Flags::passiveAckErr;
    constexpr CanPayloadBase::Flags crcDelErr = CanPayloadBase::Flags::crcDelErr;
    constexpr CanPayloadBase::Flags esi = CanPayloadBase::Flags::esi;
    int16_t newFlags = to_underlying(passiveAckErr) | to_underlying(crcDelErr);

    CanPayloadBase* payload = canPayload.get();
    payload->setFlags(newFlags);

    ASSERT_EQ(payload->getFlags(), newFlags);
    ASSERT_TRUE(payload->getFlag(passiveAckErr));
    ASSERT_TRUE(payload->getFlag(crcDelErr));
    ASSERT_FALSE(payload->getFlag(esi));

    payload->setFlag(esi, true);
    payload->setFlag(crcDelErr, false);
    newFlags = to_underlying(passiveAckErr) | to_underlying(esi);

    ASSERT_EQ(payload->getFlags(), newFlags);
    ASSERT_TRUE(payload->getFlag(passiveAckErr));
    ASSERT_FALSE(payload->getFlag(crcDelErr));
    ASSERT_TRUE(payload->getFlag(esi));

    ASSERT_EQ(dcCanPayload.getFlags(), 0u);
}

TEST_F(CanPayloadTest, Id)
{
    constexpr uint32_t newId = 123;
    CanPayloadBase* payload = canPayload.get();
    payload->setId(newId);
    ASSERT_EQ(payload->getId(), newId);
    ASSERT_EQ(dcCanPayload.getId(), 0u);
}

TEST_F(CanPayloadTest, IdMax)
{
    constexpr uint32_t newId = 0x1FFFFFFF;
    CanPayloadBase* payload = canPayload.get();
    payload->setId(newId);
    ASSERT_EQ(payload->getId(), newId);
}

TEST_F(CanPayloadTest, Rsvd)
{
    CanPayloadBase* payload = canPayload.get();
    payload->setRsvd(true);
    ASSERT_TRUE(payload->getRsvd());
    ASSERT_FALSE(dcCanPayload.getRsvd());
}

TEST_F(CanPayloadTest, Rtr)
{
    canPayload->setRtr(true);
    ASSERT_TRUE(canPayload->getRtr());
    ASSERT_FALSE(dcCanPayload.getRtr());
}

TEST_F(CanPayloadTest, Rrs)
{
    canFdPayload->setRrs(true);
    ASSERT_TRUE(canFdPayload->getRrs());
    ASSERT_FALSE(dcCanFdPayload.getRrs());
}

TEST_F(CanPayloadTest, Ide)
{
    CanPayloadBase* payload = canPayload.get();
    payload->setIde(true);
    ASSERT_TRUE(payload->getIde());
    ASSERT_FALSE(dcCanPayload.getIde());
}

TEST_F(CanPayloadTest, IdAllBits)
{
    constexpr uint32_t newId = 123;

    canPayload->setId(newId);
    canPayload->setRsvd(true);
    canPayload->setRtr(true);
    canPayload->setIde(true);
    ASSERT_EQ(canPayload->getId(), newId);

    canPayload->setId(0);
    ASSERT_TRUE(canPayload->getRsvd());
    ASSERT_TRUE(canPayload->getRtr());
    ASSERT_TRUE(canPayload->getIde());
}

TEST_F(CanPayloadTest, Crc)
{
    constexpr uint16_t crc = 0x46E5;
    canPayload->setCrc(crc);
    ASSERT_EQ(canPayload->getCrc(), crc);
    ASSERT_EQ(dcCanPayload.getCrc(), 0u);
}

TEST_F(CanPayloadTest, CrcMax)
{
    constexpr uint16_t crc = 0x7FFF;
    canPayload->setCrc(crc);
    ASSERT_EQ(canPayload->getCrc(), crc);
}

TEST_F(CanPayloadTest, CrcSupport)
{
    canPayload->setCrcSupport(true);
    ASSERT_TRUE(canPayload->getCrcSupport());
    ASSERT_EQ(dcCanPayload.getCrcSupport(), 0u);
}

TEST_F(CanPayloadTest, CrcAllBits)
{
    constexpr uint16_t crc = 0x46E5;
    canPayload->setCrc(crc);
    canPayload->setCrcSupport(true);
    ASSERT_EQ(canPayload->getCrc(), crc);

    canPayload->setCrc(0);
    ASSERT_TRUE(canPayload->getCrcSupport());
}

TEST_F(CanPayloadTest, CrcSbc)
{
    constexpr uint32_t crc = 0x1369CB;
    canFdPayload->setCrc(crc);
    ASSERT_EQ(canFdPayload->getCrc(), crc);
    ASSERT_EQ(dcCanFdPayload.getCrc(), 0u);
}

TEST_F(CanPayloadTest, CrcSbcMax)
{
    constexpr uint32_t crc = 0x1FFFFF;
    canFdPayload->setCrc(crc);
    ASSERT_EQ(canFdPayload->getCrc(), crc);
}

TEST_F(CanPayloadTest, Sbc)
{
    constexpr uint32_t sbc = 0x03;

    canFdPayload->setSbc(sbc);
    ASSERT_EQ(canFdPayload->getSbc(), sbc);
    ASSERT_EQ(dcCanFdPayload.getSbc(), 0u);
}

TEST_F(CanPayloadTest, SbcMax)
{
    constexpr uint32_t sbc = 0x07;

    canFdPayload->setSbc(sbc);
    ASSERT_EQ(canFdPayload->getSbc(), sbc);
}

TEST_F(CanPayloadTest, SbcParity)
{
    canFdPayload->setSbcParity(true);
    ASSERT_TRUE(canFdPayload->getSbcParity());
    ASSERT_EQ(dcCanFdPayload.getSbcParity(), 0u);
}

TEST_F(CanPayloadTest, SbcSupport)
{
    canFdPayload->setSbcSupport(true);
    ASSERT_TRUE(canFdPayload->getSbcSupport());
    ASSERT_EQ(dcCanFdPayload.getSbcSupport(), 0u);
}

TEST_F(CanPayloadTest, CrcSbcSupport)
{
    canFdPayload->setCrcSupport(true);
    ASSERT_TRUE(canFdPayload->getCrcSupport());
    ASSERT_EQ(dcCanFdPayload.getCrcSupport(), 0u);
}

TEST_F(CanPayloadTest, CrcSbcAllBits)
{
    constexpr uint32_t crc = 0x1369CB;
    constexpr uint32_t sbc = 0x04;

    canFdPayload->setCrc(crc);
    canFdPayload->setSbc(sbc);
    canFdPayload->setSbcParity(true);
    canFdPayload->setSbcSupport(true);
    canFdPayload->setCrcSupport(true);
    ASSERT_EQ(canFdPayload->getCrc(), crc);
    ASSERT_EQ(canFdPayload->getSbc(), sbc);

    canFdPayload->setCrc(0);
    canFdPayload->setSbc(0);
    ASSERT_TRUE(canFdPayload->getSbcParity());
    ASSERT_TRUE(canFdPayload->getSbcSupport());
    ASSERT_TRUE(canFdPayload->getCrcSupport());
}

TEST_F(CanPayloadTest, ErrorPosition)
{
    constexpr uint16_t errorPosition = 3456;

    CanPayloadBase* payload = canPayload.get();
    payload->setErrorPosition(errorPosition);
    ASSERT_EQ(payload->getErrorPosition(), errorPosition);
    ASSERT_EQ(dcCanPayload.getErrorPosition(), 0u);
}

TEST_F(CanPayloadTest, Dlc)
{
    // For data size 64, dlc is 0x0F
    constexpr uint8_t dataSize = 64;
    constexpr uint8_t dlc = 0x0F;

    CanPayloadBase* payload = canPayload.get();
    ASSERT_EQ(payload->getDataLength(), dataSize);
    ASSERT_EQ(payload->getDlc(), dlc);
    ASSERT_EQ(dcCanPayload.getDlc(), 0u);
}

TEST_F(CanPayloadTest, DataLength)
{
    CanPayloadBase* payload = canPayload.get();
    ASSERT_EQ(payload->getDataLength(), canDataSize);
    ASSERT_EQ(dcCanPayload.getDataLength(), 0u);
}

TEST_F(CanPayloadTest, Data)
{
    CanPayloadBase* payload = canPayload.get();
    ASSERT_TRUE(std::equal(data.begin(), data.end(), payload->getData()));
    ASSERT_EQ(dcCanPayload.getData(), nullptr);
}

TEST_F(CanPayloadTest, IsValidPayload)
{
    data.resize(canDataSize);
    auto message = createCanDataMessage(arbId, data);
    ASSERT_TRUE(CanPayloadBase::isValidPayload(message.data(), message.size()));
}
