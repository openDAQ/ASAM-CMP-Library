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

InterfacePayload::InterfacePayload()
    : Payload(PayloadType::ifStatMsg, minPayloadSize)
{
}

InterfacePayload::InterfacePayload(const uint8_t* data, const size_t size)
    : Payload(PayloadType::ifStatMsg, data, size)
{
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

uint16_t InterfacePayload::getStreamIdsCount() const
{
    return toUint16(getStreamIdCountPtr());
}

const uint8_t* InterfacePayload::getStreamIds() const
{
    return getStreamIdsCount() ? getStreamIdCountPtr() + sizeof(uint16_t) : nullptr;
}

uint16_t InterfacePayload::getVendorDataLength() const
{
    return toUint16(getVendorDataLengthPtr());
}

const uint8_t* InterfacePayload::getVendorData() const
{
    return getVendorDataLength() ? getVendorDataLengthPtr() + sizeof(uint16_t) : nullptr;
}

bool InterfacePayload::isValidPayload(const uint8_t* data, const size_t size)
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

const uint8_t* InterfacePayload::getStreamIdCountPtr() const
{
    return payloadData.data() + sizeof(Header);
}

const uint8_t* InterfacePayload::getVendorDataLengthPtr() const
{
    auto countPtr = getStreamIdCountPtr();
    auto count = toUint16(countPtr);
    if (count % 2)
        ++count;

    return countPtr + sizeof(uint16_t) + count;
}

uint16_t InterfacePayload::toUint16(const uint8_t* ptr) const
{
    return swapEndian(*reinterpret_cast<const uint16_t*>(ptr));
}

END_NAMESPACE_ASAM_CMP
