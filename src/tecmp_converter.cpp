#include <asam_cmp/can_payload.h>
#include <asam_cmp/capture_module_payload.h>
#include <asam_cmp/interface_payload.h>
#include <asam_cmp/lin_payload.h>
#include <asam_cmp/tecmp_can_payload.h>
#include <asam_cmp/tecmp_capture_module_payload.h>
#include <asam_cmp/tecmp_converter.h>
#include <asam_cmp/tecmp_interface_payload.h>
#include <asam_cmp/tecmp_lin_payload.h>

using ASAM::CMP::Packet;
using PacketPtr = std::shared_ptr<ASAM::CMP::Packet>;

PacketPtr TECMP::Converter::ConvertPacket(CmpHeader& header, const TecmpPayloadPtr& payload)
{
    switch (header.getMessageType())
    {
        case CmpHeader::MessageType::cmStatus:
        {
            auto packet = ConvertCaptureModulePayload(header, payload);
            if (packet && packet->isValid())
            {
                return packet;
            }
            break;
        }
        case CmpHeader::MessageType::busStatus:
        {
            auto packet = ConvertInterfacePayload(header, payload);
            if (packet && packet->isValid())
            {
                return packet;
            }
            break;
        }
        case CmpHeader::MessageType::data:
        {
            auto packet = ConvertDataPayload(header, payload);
            if (packet && packet->isValid())
            {
                return packet;
            }
            break;
        }
        case CmpHeader::MessageType::control:
        case CmpHeader::MessageType::configStatus:
        case CmpHeader::MessageType::replayData:
        case CmpHeader::MessageType::invalid:
            break;
    }
    return nullptr;
}
PacketPtr TECMP::Converter::ConvertCanPayload(CmpHeader& header, const TecmpPayloadPtr& payload)
{
    auto tecmpCanPayload = reinterpret_cast<TECMP::CanPayload*>(payload.get());
    auto packet = GetPackageFromTecmpHeader(header);
    if (tecmpCanPayload->getDlc() > 8)
    {
        return ConvertCanFdPayload(tecmpCanPayload, packet);
    }
    ASAM::CMP::CanPayload canPayload;

    canPayload.setId(tecmpCanPayload->getArbId());
    canPayload.setData(tecmpCanPayload->getData(), tecmpCanPayload->getDlc());
    canPayload.setCrc(tecmpCanPayload->getCrc());

    packet->setPayload(canPayload);

    return packet;
}

PacketPtr TECMP::Converter::ConvertCaptureModulePayload(CmpHeader& header, const TecmpPayloadPtr& payload)
{
    auto tecmpCmPayload = reinterpret_cast<TECMP::CaptureModulePayload*>(payload.get());
    auto packet = GetPackageFromTecmpHeader(header);
    ASAM::CMP::CaptureModulePayload cmPayload;
    cmPayload.setData(
        "", std::to_string(tecmpCmPayload->getSerialNumber()), tecmpCmPayload->getHwVersion(), tecmpCmPayload->getSwVersion(), {});

    packet->setPayload(cmPayload);

    return packet;
}

PacketPtr TECMP::Converter::ConvertInterfacePayload(CmpHeader& header, const TecmpPayloadPtr& payload)
{
    auto tecmpInterfacePayload = reinterpret_cast<TECMP::InterfacePayload*>(payload.get());
    auto packet = GetPackageFromTecmpHeader(header);
    ASAM::CMP::InterfacePayload interfacePayload;

    interfacePayload.setInterfaceId(tecmpInterfacePayload->getInterfaceId());
    interfacePayload.setMsgTotalRx(tecmpInterfacePayload->getMessagesTotal());
    interfacePayload.setErrorsTotalRx(tecmpInterfacePayload->getErrorsTotal());
    packet->setInterfaceId(tecmpInterfacePayload->getInterfaceId());

    packet->setPayload(interfacePayload);

    return packet;
}
PacketPtr TECMP::Converter::ConvertDataPayload(CmpHeader& header, const TecmpPayloadPtr& payload)
{
    switch (header.getDataType())
    {
        case CmpHeader::DataType::can:
        case CmpHeader::DataType::canFd:
            return ConvertCanPayload(header, payload);
            break;
        case CmpHeader::DataType::lin:
            return convertLinPayload(header, payload);
            break;
        case CmpHeader::DataType::flexRay:
        case CmpHeader::DataType::uartRs232:
        case CmpHeader::DataType::analog:
        case CmpHeader::DataType::ethernet:
        case CmpHeader::DataType::invalid:
            break;
    }

    return nullptr;
}

PacketPtr TECMP::Converter::ConvertCanFdPayload(TECMP::CanPayload* canPayload, PacketPtr packet)
{
    ASAM::CMP::CanPayload canFdPayload;

    canFdPayload.setId(canPayload->getArbId());
    canFdPayload.setData(canPayload->getData(), canPayload->getDlc());
    canFdPayload.setCrc(canPayload->getCrc());

    packet->setPayload(canFdPayload);

    return packet;
}

PacketPtr TECMP::Converter::convertLinPayload(TECMP::CmpHeader& header, const TecmpPayloadPtr& payload)
{
    auto tecmpLinPayload = reinterpret_cast<TECMP::LinPayload*>(payload.get());
    auto packet = GetPackageFromTecmpHeader(header);

    ASAM::CMP::LinPayload linPayload;

    linPayload.setLinId(tecmpLinPayload->getPid());
    linPayload.setChecksum(tecmpLinPayload->getCrc());
    linPayload.setData(tecmpLinPayload->getData(), tecmpLinPayload->getDataLength());

    packet->setPayload(linPayload);

    return packet;
}

PacketPtr TECMP::Converter::GetPackageFromTecmpHeader(const TECMP::CmpHeader& header)
{
    PacketPtr packet = std::make_shared<Packet>();
    packet->setDeviceId(header.getDeviceId());
    packet->setTimestamp(header.getTimestamp());
    packet->setInterfaceId(header.getInterfaceId());
    return packet;
}
