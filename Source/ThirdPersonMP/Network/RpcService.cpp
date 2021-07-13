// Fill out your copyright notice in the Description page of Project Settings.

#include "RpcService.h"
#include "Network/muduo/RpcChannel.h"

RpcService::RpcService(TSharedPtr<class RpcChannel> pRpcChannel)
{
    RpcServiceRegStub(pRpcChannel);
}

RpcService::~RpcService()
{
    for (auto it : m_MapStub)
    {
        if (!it.second)
        {
            delete it.second;
            it.second = nullptr;
        }
    }
}
    
::google::protobuf::Service* RpcService::GetStub(const std::string& stubName)
{
    auto itFind = m_MapStub.find(stubName);
    if (itFind == m_MapStub.end()) return nullptr;
    return itFind->second;
}
