#pragma once

#include <google/protobuf/service.h>
#include "down_pointer_cast.h"
#include "noncopyable.h"
#include "dynamic_object.h"

namespace google {
namespace protobuf {

// Defined in other files.
class Descriptor;            // descriptor.h
class ServiceDescriptor;     // descriptor.h
class MethodDescriptor;      // descriptor.h
class Message;               // message.h
typedef ::std::shared_ptr<Message> MessagePtr;
typedef ::std::shared_ptr<const Message> ConstMessagePtr;

}  // namespace protobuf
}  // namespace google


namespace muduo
{
namespace net
{

// Defined in this file.
// class Service;
class RpcController;
typedef ::std::function<void(const ::google::protobuf::Message*)> RpcDoneCallback;
// typedef ::std::map<std::string, Service*> ServiceMap;


class RpcChannelBase : noncopyable
{
public:
    virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            const ::google::protobuf::MessagePtr& request,
                            const ::google::protobuf::MessagePtr& response,
                            void* args) {}
};


// Abstract base interface for protocol-buffer-based RPC services.  Services
// themselves are abstract interfaces (implemented either by servers or as
// stubs), but they subclass this base interface.  The methods of this
// interface can be used to call the methods of the Service without knowing
// its exact type at compile time (analogous to Reflection).
class Service : noncopyable
              , public DynamicObject
{
 public:
  Service() {}
  virtual ~Service() {}

  typedef ::muduo::net::RpcDoneCallback RpcDoneCallback;

  // Get the ServiceDescriptor describing this service and its methods.
  virtual const ::google::protobuf::ServiceDescriptor* GetDescriptor() = 0;

  // Call a method of the service specified by MethodDescriptor.  This is
  // normally implemented as a simple switch() that calls the standard
  // definitions of the service's methods.
  //
  // Preconditions:
  // * method->service() == GetDescriptor()
  // * request and response are of the exact same classes as the objects
  //   returned by GetRequestPrototype(method) and
  //   GetResponsePrototype(method).
  // * After the call has started, the request must not be modified and the
  //   response must not be accessed at all until "done" is called.
  // * "controller" is of the correct type for the RPC implementation being
  //   used by this Service.  For stubs, the "correct type" depends on the
  //   RpcChannel which the stub is using.  Server-side Service
  //   implementations are expected to accept whatever type of RpcController
  //   the server-side RPC implementation uses.
  //
  // Postconditions:
  // * "done" will be called when the method is complete.  This may be
  //   before CallMethod() returns or it may be at some point in the future.
  // * If the RPC succeeded, "response" contains the response returned by
  //   the server.
  // * If the RPC failed, "response"'s contents are undefined.  The
  //   RpcController can be queried to determine if an error occurred and
  //   possibly to get more information about the error.
  virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                          const ::google::protobuf::MessagePtr& request,
                          const ::google::protobuf::MessagePtr& response,
                          void* args) {}

  // CallMethod() requires that the request and response passed in are of a
  // particular subclass of Message.  GetRequestPrototype() and
  // GetResponsePrototype() get the default instances of these required types.
  // You can then call Message::New() on these instances to construct mutable
  // objects which you can then pass to CallMethod().
  //
  // Example:
  //   const MethodDescriptor* method =
  //     service->GetDescriptor()->FindMethodByName("Foo");
  //   Message* request  = stub->GetRequestPrototype (method)->New();
  //   Message* response = stub->GetResponsePrototype(method)->New();
  //   request->ParseFromString(input);
  //   service->CallMethod(method, *request, response, callback);
  virtual const ::google::protobuf::Message& GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const = 0;
  virtual const ::google::protobuf::Message& GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const = 0;

  virtual void DoneCallback(const ::google::protobuf::MethodDescriptor* method,
                            const ::google::protobuf::MessagePtr& request,
                            const ::google::protobuf::MessagePtr& response) {}
  
  virtual void TimeOut(const ::google::protobuf::MethodDescriptor* method,
                       const ::google::protobuf::MessagePtr& request) {}
};

}
}

