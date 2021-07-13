#pragma once

#include "CoreMinimal.h"
#include <atomic>

DECLARE_DELEGATE(FTCPSocketRunnableConnectedDelegate);
DECLARE_DELEGATE(FTCPSocketRunnableNewDataReceivedDelegate);
DECLARE_DELEGATE(FTCPSocketRunnableConnectionErrorDelegate);

/**
 * IO线程的Runnable
 */
class FTCPSocketRunnable : public FRunnable, public TSharedFromThis<FTCPSocketRunnable>
{
public:
    FTCPSocketRunnable(const FString& IP, int32 Port, TQueue<TArray<uint8>, EQueueMode::Spsc>& SendBuffer, TArray<uint8>& RecvData);
    virtual ~FTCPSocketRunnable();

    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    virtual void Exit() override;

    FORCEINLINE FTCPSocketRunnableNewDataReceivedDelegate& GetNewDataReceivedDelegate() { return NewDataReceivedDelegate; }
    FORCEINLINE FTCPSocketRunnableConnectionErrorDelegate& GetConnectionErrorDelegate() { return ConnectionErrorDelegate; }
    FORCEINLINE FTCPSocketRunnableConnectedDelegate& GetConnectedDelegate() { return ConnectedDelegate; }

private:
    FString IP;
    int32 Port;
    bool bConnectionError;                                              // 是否出现错误情况，在read()或recv()返回错误时会置true
    std::atomic<bool> bManuallyStop;                                    // 是否执行过Stop()
    TQueue<TArray<uint8>, EQueueMode::Spsc>& SendBuffer;                // 输出缓存，IO线程只会取出
    TArray<uint8>& RecvData;                                            // 输入缓存，IO线程只会写入
    TSharedPtr<class FSocket> SocketPtr;

    FTCPSocketRunnableNewDataReceivedDelegate NewDataReceivedDelegate;
    FTCPSocketRunnableConnectionErrorDelegate ConnectionErrorDelegate;
    FTCPSocketRunnableConnectedDelegate ConnectedDelegate;
};

