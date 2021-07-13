#include "TcpSocketRunnable.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "Async/Async.h"
#include "Networking.h"

FTCPSocketRunnable::FTCPSocketRunnable( const FString& IP, 
                                        int32 Port, 
                                        TQueue<TArray<uint8>, EQueueMode::Spsc>& SendBuffer, 
                                        TArray<uint8>& RecvData)
    : IP(IP)
    , Port(Port)
    , bConnectionError(false)
    , bManuallyStop(false)
    , SendBuffer(SendBuffer)
    , RecvData(RecvData)
{

}

FTCPSocketRunnable::~FTCPSocketRunnable()
{
    Stop();
}

bool FTCPSocketRunnable::Init()
{
    SocketPtr = MakeShareable(ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("FTCPSocketRunnable"), false));
    if (!SocketPtr.IsValid())
    {
        return false;
    }

    auto SocketSubSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    auto ResolveInfo = SocketSubSystem->GetHostByName(TCHAR_TO_ANSI(*IP));
    while (!ResolveInfo->IsComplete());

    if (ResolveInfo->GetErrorCode() != 0)
    {
        return false;
    }

    const FInternetAddr& Addr = ResolveInfo->GetResolvedAddress();
    TSharedRef<FInternetAddr> InternetAddr = SocketSubSystem->CreateInternetAddr();
    uint32 ip;
    Addr.GetIp(ip);
    InternetAddr->SetIp(ip);
    InternetAddr->SetPort(Port);
    auto Success = SocketPtr->Connect(*InternetAddr);

    if (!Success)
    {
        ConnectionErrorDelegate.ExecuteIfBound();
        return false;
    }

    ConnectedDelegate.ExecuteIfBound();

    return true;
}

uint32 FTCPSocketRunnable::Run()
{
    while ( !bManuallyStop 
            && SocketPtr.IsValid() 
            && ESocketConnectionState::SCS_Connected == SocketPtr->GetConnectionState())
    {
        // 发数据
        TArray<uint8> Buffer;
        while ( !bManuallyStop 
                && !bConnectionError 
                && SendBuffer.Peek(Buffer))
        {
            uint8* RawData = Buffer.GetData();
            int32 TotalCount = Buffer.Num();
            int32 SendedCount = 0;
            int32 BytesSent = 0;
            do
            {
                bool Success = SocketPtr->Send(RawData + SendedCount, TotalCount - SendedCount, BytesSent);
                if (!Success)
                {
                    bConnectionError = true;
                    break;
                }
                SendedCount += BytesSent;
            } while (SendedCount < TotalCount);

            if (!bConnectionError)
            {
                SendBuffer.Pop();
            }
        }

        if (bManuallyStop || bConnectionError)
        {
            return 0;
        }

        // 收数据
        uint32 PendingDataSize = 0;
        if (SocketPtr->HasPendingData(PendingDataSize))
        {
            auto OriginalNum = RecvData.Num();
            int32 BytesRead = 0;
            uint32 BytesReadTotal = 0;

            RecvData.SetNum(OriginalNum + PendingDataSize, false);

            uint8* StartIdx = RecvData.GetData() + OriginalNum;

            bool Success = false;

            do
            {
                Success = SocketPtr->Recv(StartIdx + BytesReadTotal, PendingDataSize - BytesReadTotal, BytesRead);
                if (!Success)
                {
                    bConnectionError = true;
                    break;
                }
                BytesReadTotal += BytesRead;
            } while (BytesReadTotal < PendingDataSize);

            if (!bManuallyStop && !bConnectionError)
            {
                NewDataReceivedDelegate.ExecuteIfBound();
            }
        }

        if (bManuallyStop || bConnectionError)
        {
            return 0;
        }
    }

    return 0;
}

void FTCPSocketRunnable::Stop()
{
    bManuallyStop = true;
}

void FTCPSocketRunnable::Exit()
{
    if (SocketPtr.IsValid())
    {
        if (bConnectionError && !bManuallyStop)
        {
            ConnectionErrorDelegate.ExecuteIfBound();
        }
        SocketPtr->Close();
    }
}
