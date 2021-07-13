#pragma once

#include "CoreMinimal.h"
#include "TcpSocketRunnable.h"

DECLARE_DELEGATE_OneParam(FTCPSocketConnectDelegate, bool);
DECLARE_DELEGATE(FTCPSocketDisconnectedDelegate);
DECLARE_DELEGATE_OneParam(FTCPSocketReceivedMessageDelegate, TArray<uint8>&);

class FRunnableThread;

class FTCPSocket : public TSharedFromThis<FTCPSocket>
{
public:
    FTCPSocket(const FTCPSocketConnectDelegate& ConnectDelegate, const FTCPSocketDisconnectedDelegate& DisconnectedDelegate, const FTCPSocketReceivedMessageDelegate& ReceivedMessageDelegate);
    virtual ~FTCPSocket();

    void Connect(const FString& IP, int32 Port);
    void Disconnect();
    void Send(uint8* Data, int32 Count);

private:
    TSharedPtr<FRunnableThread> SocketThreadPtr;                    // IO线程
    TSharedPtr<FTCPSocketRunnable> SocketRunnablePtr;

    TQueue<TArray<uint8>, EQueueMode::Spsc> SendBuffer;             // 发送缓存，逻辑线程只会写入
    TArray<uint8> RecvData;                                         // 接收缓存，逻辑线程只会读

    FTCPSocketConnectDelegate ConnectDelegate;
    FTCPSocketDisconnectedDelegate DisconnectedDelegate;
    /*This Delegate is Called in WorkingThread*/
    FTCPSocketReceivedMessageDelegate ReceivedMessageDelegate;
};