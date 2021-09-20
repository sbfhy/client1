#include "RpcChannel.h"

#include "System/Subsystem/MgrMessage.h"
#include "Base/Log/Logger.h"
#include "message/common/rpc.pb.h"
#include "Network/muduo/define_service.h"

#include "service/service_include.pb.h"
#include "service/c2g_user.pb.h"
#include <google/protobuf/descriptor.h>

using namespace muduo::net;
using namespace CMD;

RpcChannel::RpcChannel()
{
    LLOG_NET("%s %p", *FString("ctor RpcChannel"), this);
}

RpcChannel::RpcChannel(UMgrMessage* pMgrMessage) 
    : m_pMgrMessage(pMgrMessage)
{
    RpcMessage msg;
    LLOG_NET("%s %p", *FString("ctor RpcChannel"), this);
}

RpcChannel::~RpcChannel()
{
}

void RpcChannel::OnMessage(const TArray<uint8>& Data)
{
    RpcMessage msg;
    msg.ParseFromArray(Data.GetData(), Data.Num());

    if (msg.type() == MSGTYPE_RESPONSE)
    {
        stubHandleResponseMsg(msg);
    }
    else if (msg.type() == MSGTYPE_REQUEST)
    {
        serviceHandleRequestMsg(msg);
    }
}

void RpcChannel::serviceHandleRequestMsg(const RpcMessage& message) // Service处理request消息
{
    if (!m_pMgrMessage) return;

    ErrorCode errorCode = ERR_NO_ERROR;
    auto funcErrorCode = [&]()
    {
        if (errorCode != ERR_NO_ERROR) // 告诉请求方处理request时出现错误
        {
            RpcMessage response;
            response.set_type(MSGTYPE_RESPONSE);
            response.set_id(message.id());
            response.set_error(errorCode);
            m_pMgrMessage->SendMessage(response);
        }
    };

    ServicePtr pService = m_pMgrMessage->GetServicePtr(message.service());
    if (!pService)
    {
        errorCode = ERR_NO_SERVICE;
        return funcErrorCode();
    }
    const google::protobuf::ServiceDescriptor* desc = pService->GetDescriptor();
    const google::protobuf::MethodDescriptor* method = desc->method(message.method());
    if (!method)
    {
        errorCode = ERR_NO_METHOD;
        return funcErrorCode();
    }

    // FIXME: can we move deserialization to other thread?
    ::google::protobuf::MessagePtr request(pService->GetRequestPrototype(method).New());
    if (!request->ParseFromString(message.request()))
    {
        errorCode = ERR_WRONG_PROTO;
        return funcErrorCode();
    }
    // {LDBG("M_NET") << request->ShortDebugString();}

    int64_t id = message.id(); (void)id;
    ::google::protobuf::MessagePtr response;

    // 如果response类型是EmptyResponse，就不发回包
    if (&pService->GetResponsePrototype(method) != &EmptyResponse::default_instance())
    {
        response = ::google::protobuf::MessagePtr(pService->GetResponsePrototype(method).New());
    }

    // 调用处理函数
    pService->CallMethod(method, request, response);

    if (response)                                   // FIXME: delay response
    {
        //m_pMgrMessage->SendMessage(response);     // 发送回包
    }

    funcErrorCode();
}

void RpcChannel::stubHandleResponseMsg(const RpcMessage& message)    // Stub处理response消息
{
    LLOG_NET("service:%d, method:%d, id:%llu, accid:%llu, from:%d, to:%d", message.service(), message.method(), message.id(), message.accid(), message.from(), message.to());
    if (!m_pMgrMessage || message.response() == "")
        return;
    QWORD id = message.id();

    OutstandingCall out;
    bool found = false;

    const auto it = m_outstandings.find(id);
    if (it != m_outstandings.end())
    {
        out = it->second;
        m_outstandings.erase(it);
        found = true;
    }

    if (!found)
    {
        //LOG_WARN << "[处理response-没找到对应id] " << message.ShortDebugString();
        return;
    }
    //if (conn_ && conn_->GetLoop())
    //{
    //    conn_->GetLoop()->Cancel(out.timerId);  // 删除超时回调的定时器
    //}

    const ServicePtr pService = m_pMgrMessage->GetServicePtr(out.serviceType);
    if (!pService || !pService->GetDescriptor())
    {
        //LOG_WARN << "[处理response-没找到service] " << message.ShortDebugString();
        return;
    }
    const auto methodDesc = pService->GetDescriptor()->method(out.methodIndex);
    if (!methodDesc) return;

    // FIXME: can we move deserialization to other thread?
    ::google::protobuf::MessagePtr response(
        pService->GetResponsePrototype(methodDesc).New());
    response->ParseFromString(message.response());
    pService->DoneCallback(methodDesc, out.request, response);  // 调用Stub中的回调
}

void RpcChannel::Send(const ::google::protobuf::MessagePtr& request)
{
    //// FIXME: can we move serialization to IO thread? 
    if (!m_pMgrMessage || !request) return;
    const SServiceInfo* serviceInfo = m_pMgrMessage->GetServiceInfo(request->GetDescriptor());
    if (!serviceInfo) return;

    RpcMessage message;
    message.set_type(MSGTYPE_REQUEST);
    message.set_id(++ m_id);
    message.set_service(serviceInfo->serviceType);
    message.set_method(serviceInfo->methodIndex);
    message.set_from(ENUM::ESERVERTYPE_CLIENT);
    message.set_to(serviceInfo->to);
    message.set_accid(m_pMgrMessage->GetAccid());
    message.set_request(request->SerializeAsString());  // FIXME: error check

    OutstandingCall out = { request,
                            serviceInfo->serviceType,
                            serviceInfo->methodIndex,
                            //conn_->GetLoop()->RunAfter(5.0, std::bind(&RpcChannel::requestTimeOut, shared_from_this(), id)) 
    };
    
    m_outstandings[m_id] = out;

    LLOG_NET("service:%d, method:%d, accid:%llu, from:%d, to:%d", serviceInfo->serviceType, serviceInfo->methodIndex, message.accid(), message.from(), message.to());
    if (m_pMgrMessage)
    {
        const std::string msgStr = message.SerializeAsString();
        m_pMgrMessage->SendMessage(msgStr);
    }
}

