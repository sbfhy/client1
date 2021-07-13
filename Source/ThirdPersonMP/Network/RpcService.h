// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <map>

namespace google {
namespace protobuf {

class Service;

}   // namespace protobuf
}   // namespace google

/**
 * 
 */
class THIRDPERSONMP_API RpcService
{
public:
    RpcService() {}
    RpcService(TSharedPtr<class RpcChannel> );
    virtual ~RpcService();

    ::google::protobuf::Service* GetStub(const std::string&);

private:
    void RpcServiceRegStub(TSharedPtr<class RpcChannel> );

private:
    std::map<std::string, ::google::protobuf::Service*> m_MapStub;
    std::map<std::string, ::google::protobuf::Service*> m_MapService;
};
