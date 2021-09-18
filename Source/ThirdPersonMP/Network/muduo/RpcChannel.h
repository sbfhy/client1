#pragma once

#include "service/service_enum.pb.h"
#include "service.h"
#include <google/protobuf/service.h>
#include <map>

namespace google {
namespace protobuf {

class Descriptor;            // descriptor.h
class ServiceDescriptor;     // descriptor.h
class MethodDescriptor;      // descriptor.h
class Message;               // message.h
class Service;

typedef std::shared_ptr<Message> MessagePtr;

}   // namespace protobuf
}   // namespace google


namespace CMD {

class RpcMessage;

}   // namespace CMD 


class UMgrMessage;


class RpcChannel : public muduo::net::RpcChannelBase
                 , public std::enable_shared_from_this<RpcChannel>
{
    typedef CMD::RpcMessage  RpcMessage;

public:
    RpcChannel();
    RpcChannel(UMgrMessage* );
    virtual ~RpcChannel();

public:
    typedef std::map<std::string, ::google::protobuf::Service*> ServiceMap;

    void SetTCPSocketPtr(TSharedPtr<class FTCPSocket> ptr) { m_TCPSocketPtr = ptr; }
    void OnMessage(const TArray<uint8>& Data);

    void Send(const ::google::protobuf::MessagePtr& request);

private:
    void serviceHandleRequestMsg(const RpcMessage& message);    // Service处理request消息
    void stubHandleResponseMsg(const RpcMessage& message);      // Stub处理response消息

private:
    struct OutstandingCall
    {
        ::google::protobuf::MessagePtr request = nullptr;
        ENUM::EServiceType serviceType = ENUM::SERVICETYPE_MIN;
        int methodIndex = -1;
        //TimerId timerId;
    };

    uint64_t m_id = 0;                                  // 自增Id，回调函数需要
    std::map<uint64_t, OutstandingCall> m_outstandings;
    
    
    TSharedPtr<class FTCPSocket> m_TCPSocketPtr;
    UMgrMessage* m_pMgrMessage;
};


