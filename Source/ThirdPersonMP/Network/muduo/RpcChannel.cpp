#pragma once

#include "RpcChannel.h"
#include "RpcCodec.h"
#include "System/Subsystem/MgrMessage.h"
#include "Base/Log/Logger.h"
#include <pb/rpc.pb.h>

using namespace muduo::net;

RpcChannel::RpcChannel()
{
}

RpcChannel::RpcChannel(UMgrMessage* pMgrMessage)
    : m_pMgrMessage(pMgrMessage)
{
    LLOG_NET("%s %p", *FString("ctor RpcChannel"), this);
}

RpcChannel::~RpcChannel()
{
}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done)
{
    RpcMessage message;
    message.set_type(MSGTYPE_REQUEST);
    message.set_id(m_id);
    message.set_service(method->service()->full_name());
    message.set_method(method->index());
    message.set_request(request->SerializeAsString()); // FIXME: error check

    OutstandingCall out = { response, done };
    m_outstandings[m_id++] = out;

    LLOG_NET("%llu, service:%s, method:%d, request:%s, isNull:%d, mgrAddr:%p", message.id(), *FString(message.service().c_str()), message.method(), *FString(message.request().c_str()), m_pMgrMessage==nullptr, m_pMgrMessage);

    if (m_pMgrMessage)
    {
        const std::string msgStr = message.SerializeAsString();


        m_pMgrMessage->SendMessage(msgStr);
        //m_pMgrMessage->SendMessage(std::string("test ----  "));
    }
}

void RpcChannel::OnMessage(const TArray<uint8>& Data)
{
    LLOG_NET(" %s", *FString(reinterpret_cast<const char*>(Data.GetData())));
    RpcMessage msg;
    msg.ParseFromArray(Data.GetData(), Data.Num());
    LLOG_NET(" %s", *FString(msg.DebugString().c_str()));

    if (msg.type() == MSGTYPE_RESPONSE)
    {
        OutstandingCall out = { nullptr, nullptr };
        uint64_t id = msg.id();
        auto itFind = m_outstandings.find(id);
        if (itFind != m_outstandings.end())
        {
            out = itFind->second;
            m_outstandings.erase(itFind);
        }
        else
        {
            // ERROR
        }

        if (out.response)
        {
            TSharedPtr<google::protobuf::Message> response(out.response);
            out.response->ParseFromString(msg.response());
            if (out.done)
            {
                out.done->Run();
            }
        }
    }
    else if (msg.type() == MSGTYPE_REQUEST)
    {
        // TODO 
    }
}
