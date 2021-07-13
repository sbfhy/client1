#pragma once

#include <google/protobuf/service.h>
#include <map>

namespace google {
namespace protobuf {
    
class Descriptor;            // descriptor.h
class ServiceDescriptor;     // descriptor.h
class MethodDescriptor;      // descriptor.h
class Message;               // message.h
class Closure;
class RpcController;
class Service;

}  // namespace protobuf
}  // namespace google

class RpcChannel : public ::google::protobuf::RpcChannel
{
public:
    RpcChannel();
    RpcChannel(class UMgrMessage*);
    virtual ~RpcChannel();

    // Call the given method of the remote service.  The signature of this
    // procedure looks the same as Service::CallMethod(), but the requirements
    // are less strict in one important way:  the request and response objects
    // need not be of any specific class as long as their descriptors are
    // method->input_type() and method->output_type().
    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                    ::google::protobuf::RpcController* controller,
                    const ::google::protobuf::Message* request,
                    ::google::protobuf::Message* response,
                    ::google::protobuf::Closure* done) override;

public:
    typedef std::map<std::string, ::google::protobuf::Service*> ServiceMap;

    void SetTCPSocketPtr(TSharedPtr<class FTCPSocket> ptr) { m_TCPSocketPtr = ptr; }
    void OnMessage(const TArray<uint8>& Data);

private:
    struct OutstandingCall
    {
        ::google::protobuf::Message* response;
        ::google::protobuf::Closure* done;
    };

private:
    UMgrMessage* m_pMgrMessage{ nullptr };
    TSharedPtr<class FTCPSocket> m_TCPSocketPtr;

    uint64_t m_id = 0;                                  // 自增Id，回调函数需要
    std::map<uint64_t, OutstandingCall> m_outstandings;
};


