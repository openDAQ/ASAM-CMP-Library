#include <asam_cmp/interface_payload.h>

BEGIN_NAMESPACE_ASAM_CMP

uint32_t InterfacePayload::Header::getInterfaceId() const
{
    return swapEndian(interfaceId);
}

void InterfacePayload::Header::setInterfaceId(const uint32_t id)
{
    interfaceId = swapEndian(id);
}

uint32_t InterfacePayload::Header::getMsgTotalRx() const
{
    return swapEndian(msgTotalRx);
}

void InterfacePayload::Header::setMsgTotalRx(const uint32_t msgTotal)
{
    msgTotalRx = swapEndian(msgTotal);
}

uint32_t InterfacePayload::Header::getMsgTotalTx() const
{
    return swapEndian(msgTotalTx);
}

void InterfacePayload::Header::setMsgTotalTx(const uint32_t msgTotal)
{
    msgTotalTx = swapEndian(msgTotal);
}

uint32_t InterfacePayload::Header::getMsgDroppedRx() const
{
    return swapEndian(msgDroppedRx);
}

void InterfacePayload::Header::setMsgDroppedRx(const uint32_t msgDropped)
{
    msgDroppedRx = swapEndian(msgDropped);
}

uint32_t InterfacePayload::Header::getMsgDroppedTx() const
{
    return swapEndian(msgDroppedTx);
}

void InterfacePayload::Header::setMsgDroppedTx(const uint32_t msgDropped)
{
    msgDroppedTx = swapEndian(msgDropped);
}

uint32_t InterfacePayload::Header::getErrorsTotalRx() const
{
    return swapEndian(errorsTotalRx);
}

void InterfacePayload::Header::setErrorsTotalRx(const uint32_t errorsTotal)
{
    errorsTotalRx = swapEndian(errorsTotal);
}

uint32_t InterfacePayload::Header::getErrorsTotalTx() const
{
    return swapEndian(errorsTotalTx);
}

void InterfacePayload::Header::setErrorsTotalTx(const uint32_t errorsTotal)
{
    errorsTotalTx = swapEndian(errorsTotal);
}

uint8_t InterfacePayload::Header::getInterfaceType() const
{
    return interfaceType;
}

void InterfacePayload::Header::setInterfaceType(const uint8_t ifType)
{
    interfaceType = ifType;
}

InterfacePayload::InterfaceStatus InterfacePayload::Header::getInterfaceStatus() const
{
    return static_cast<InterfaceStatus>(interfaceStatus);
}

void InterfacePayload::Header::setInterfaceStatus(const InterfaceStatus status)
{
    interfaceStatus = to_underlying(status);
}

uint32_t InterfacePayload::Header::getFeatureSupportBitmask() const
{
    return swapEndian(featureSupportBitmask);
}

void InterfacePayload::Header::setFeatureSupportBitmask(const uint32_t bitmask)
{
    featureSupportBitmask = swapEndian(bitmask);
}

InterfacePayload::InterfacePayload(const uint8_t* data, const size_t size)
    : Payload(Payload::Type::ifStatMsg, data, size)
{
    uint8_t* ptr = payloadData.data() + sizeof(Header);
    uint16_t length = swapEndian(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += sizeof(length);
    streamIds = std::string_view{reinterpret_cast<const char*>(ptr), length};
    ptr += length;
    if (length % 2)
        ++ptr;
    length = swapEndian(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += sizeof(length);
    vendorData = std::string_view{reinterpret_cast<const char*>(ptr), length};
}

uint32_t InterfacePayload::getInterfaceId() const
{
    return getHeader()->getInterfaceId();
}

void InterfacePayload::setInterfaceId(const uint32_t id)
{
    getHeader()->setInterfaceId(id);
}

uint32_t InterfacePayload::getMsgTotalRx() const
{
    return getHeader()->getMsgTotalRx();
}

void InterfacePayload::setMsgTotalRx(const uint32_t msgTotal)
{
    getHeader()->setMsgTotalRx(msgTotal);
}

uint32_t InterfacePayload::getMsgTotalTx() const
{
    return getHeader()->getMsgTotalTx();
}

void InterfacePayload::setMsgTotalTx(const uint32_t msgTotal)
{
    getHeader()->setMsgTotalTx(msgTotal);
}

uint32_t InterfacePayload::getMsgDroppedRx() const
{
    return getHeader()->getMsgDroppedRx();
}

void InterfacePayload::setMsgDroppedRx(const uint32_t msgDropped)
{
    getHeader()->setMsgDroppedRx(msgDropped);
}

uint32_t InterfacePayload::getMsgDroppedTx() const
{
    return getHeader()->getMsgDroppedTx();
}

void InterfacePayload::setMsgDroppedTx(const uint32_t msgDropped)
{
    getHeader()->setMsgDroppedTx(msgDropped);
}

uint32_t InterfacePayload::getErrorsTotalRx() const
{
    return getHeader()->getErrorsTotalRx();
}

void InterfacePayload::setErrorsTotalRx(const uint32_t errorsTotal)
{
    getHeader()->setErrorsTotalRx(errorsTotal);
}

uint32_t InterfacePayload::getErrorsTotalTx() const
{
    return getHeader()->getErrorsTotalTx();
}

void InterfacePayload::setErrorsTotalTx(const uint32_t errorsTotal)
{
    getHeader()->setErrorsTotalTx(errorsTotal);
}

uint8_t InterfacePayload::getInterfaceType() const
{
    return getHeader()->getInterfaceType();
}

void InterfacePayload::setInterfaceType(const uint8_t ifType)
{
    getHeader()->setInterfaceType(ifType);
}

InterfacePayload::InterfaceStatus InterfacePayload::getInterfaceStatus() const
{
    return getHeader()->getInterfaceStatus();
}

void InterfacePayload::setInterfaceStatus(const InterfaceStatus status)
{
    getHeader()->setInterfaceStatus(status);
}

uint32_t InterfacePayload::getFeatureSupportBitmask() const
{
    return getHeader()->getFeatureSupportBitmask();
}

void InterfacePayload::setFeatureSupportBitmask(const uint32_t bitmask)
{
    getHeader()->setFeatureSupportBitmask(bitmask);
}

uint16_t InterfacePayload::getStreamIdsSize() const
{
    return static_cast<uint16_t>(streamIds.size());
}

const uint8_t* InterfacePayload::getStreamIds() const
{
    return reinterpret_cast<const uint8_t*>(streamIds.data());
}

uint16_t InterfacePayload::getVendorDataSize() const
{
    return static_cast<uint16_t>(vendorData.size());
}

const uint8_t* InterfacePayload::getVendorData() const
{
    return reinterpret_cast<const uint8_t*>(vendorData.data());
}

bool InterfacePayload::isValidPayload([[maybe_unused]] const uint8_t* data, const size_t size)
{
    auto header = reinterpret_cast<const Header*>(data);
    return (size >= sizeof(Header) && header->getInterfaceStatus() <= InterfaceStatus::disabled);
}

const InterfacePayload::Header* InterfacePayload::getHeader() const
{
    return reinterpret_cast<const Header*>(payloadData.data());
}

InterfacePayload::Header* InterfacePayload::getHeader()
{
    return reinterpret_cast<Header*>(payloadData.data());
}

END_NAMESPACE_ASAM_CMP
