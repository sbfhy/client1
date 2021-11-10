#pragma once

#include <memory>
#include <map>
#include <array>
#include <string>

#include "service/service_enum.pb.h"
#include "Base/Define/DefineVariable.h"

namespace google {
namespace protobuf {

class Service;
class Descriptor;
class Message;

typedef std::shared_ptr<Message> MessagePtr;

}  // namespace protobuf
}  // namespace google

namespace muduo {
namespace net {

class Service;

} // namespace net
} // namespace muduo

typedef std::shared_ptr<::muduo::net::Service> ServicePtr;
typedef std::array<ServicePtr, ENUM::SERVICETYPE_MAX> TArrayService;


struct SServiceInfo
{
    ENUM::EServiceType serviceType = ENUM::SERVICETYPE_MIN;
    int methodIndex = -1;
    ENUM::EServerType from{ ENUM::ESERVERTYPE_MIN };
    ENUM::EServerType to{ ENUM::ESERVERTYPE_MIN };
};
typedef std::map<const ::google::protobuf::Descriptor*, SServiceInfo> TMapDescriptor2ServiceInfo;


// typedef std::function<void( const ::google::protobuf::MethodDescriptor* method,
//                             const ::google::protobuf::MessagePtr& request,
//                             const ::google::protobuf::MessagePtr& response)> TFuncDelayResponse;

// struct SDelayInfo
// {
//     ::google::protobuf::MessagePtr response;
//     TFuncDelayResponse funcDelayResponse;
// };
// typedef std::map<QWORD, SDelayInfo> TMapQWORD2DelayInfo;


struct OutstandingCall
{
    ::google::protobuf::MessagePtr request = nullptr;
    ENUM::EServiceType serviceType = ENUM::SERVICETYPE_MIN;
    int methodIndex = -1;
    //TimerId timerId;
};


class RpcChannel;
typedef std::shared_ptr<RpcChannel> RpcChannelPtr;

struct SDelayResponse               // delay response
{
    ENUM::EServerType from = ENUM::ESERVERTYPE_MIN;
    ::google::protobuf::MessagePtr response = nullptr;
    QWORD delayResponseId = 0;
    QWORD msgId = 0;
    QWORD accid = 0;
};

struct SRpcChannelMethodArgs        // method args
{
    bool bDelay {false};
    ENUM::EServerType from {ENUM::ESERVERTYPE_MIN};
    QWORD msgId {0};
    QWORD accid {0};
};
