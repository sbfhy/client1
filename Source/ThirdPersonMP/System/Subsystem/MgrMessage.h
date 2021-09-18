// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/Subsystem/MgrBaseGameInstanceSubsystem.h"
#include "Network/TcpSocket.h"
#include "Templates/SharedPointer.h"
#include "Network/muduo/define_service.h"
#include <map>

#include "MgrMessage.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnServerConnectedEvent);
DECLARE_DYNAMIC_DELEGATE(FOnServerDisconnectedEvent);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnServerReceiveEvent, const TArray<uint8>&, Data);

namespace CMD {
    class RpcMessage;
}   // CMD 


/**
 * 
 */
UCLASS(BlueprintType)
class THIRDPERSONMP_API UMgrMessage : public UMgrBaseGameInstanceSubsystem
{
    GENERATED_BODY()

public:    
    typedef CMD::RpcMessage  RpcMessage;

    UMgrMessage();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // 调用internal 中间处理函数
    UFUNCTION(BlueprintCallable)
        void Connect(const FString& Host);
    UFUNCTION(BlueprintCallable)
        void Disconnect();    
    
    void Send(const ::google::protobuf::MessagePtr& request);   // 调用RpcChannel::Send
    void SendMessage(const std::string& msg);
    void SendMessage(const CMD::RpcMessage& msg);
    
    // 供外部使用的回调函数
    UPROPERTY()
        FOnServerConnectedEvent OnServerConnectedDelegate;
    UPROPERTY()
        FOnServerDisconnectedEvent OnServerDisconnectedDelegate;
    UPROPERTY()
        FOnServerReceiveEvent OnServerReceiveDelegate;
    
    UFUNCTION()
        void TickMessageQueue(float DeltaTime);                 // 检查MessageQueue里是否有数据

private:
    FDelegateHandle MessageDelegateHandler;
    
    // 传到TcpSocket里的回调
    FTCPSocketConnectDelegate ConnectDelegate;
    FTCPSocketDisconnectedDelegate DisconnectedDelegate;
    FTCPSocketReceivedMessageDelegate ReceivedMessageDelegate;  // decoder, 负责解包，然后填到MessageQueue里

    // 中间处理函数，调用Socket 
    TFunction<void(const FString&)> InternalConnectFunc;        // Connect的中间处理函数，负责绑定MessageDelegateHandler，并把IP和Port拆开  
    TFunction<void()> InternalDisconnectFunc;                   // Disconnext的中间处理函数，负责解绑MessageDelegateHandler，并清空MessageQueue  
    TFunction<void(const std::string&)> InternalSendPbcFunc;    // encoder，发送消息的中间函数, 负责加上包头(length(2)|protoName(2))  

private:
    const static uint32_t kHeaderLen = sizeof(uint32_t);

    TQueue<TArray<uint8>, EQueueMode::Spsc> MessageQueue;       // 收到的消息队列  

    TSharedPtr<FTCPSocket> TCPSocketPtr;
    TSharedPtr<class RpcChannel>  m_RpcChannelPtr;


    // Service相关
public:
    const ServicePtr GetServicePtr(ENUM::EServiceType) const;
    const SServiceInfo* GetServiceInfo(const ::google::protobuf::Descriptor* requestDesc) const;
    const ::google::protobuf::ServiceDescriptor* GetServiceDescriptor(ENUM::EServiceType) const;
    const ::google::protobuf::MethodDescriptor* GetMethodDescriptor(ENUM::EServiceType, int methodIdx) const;
    QWORD GetAccid() const { return m_accid; }
    void SetAccid(QWORD accid) { m_accid = accid; }
private:
    void registerService();
    void getServiceFromTo(const std::string& serviceTypeName, ENUM::EServerType &from, ENUM::EServerType &to);
private:
    TArrayService m_arrayService;
    TMapDescriptor2ServiceInfo m_mapRequest2ServiceInfo;
    QWORD m_accid{ 0 };
};

