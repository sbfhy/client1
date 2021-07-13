
//#include "Logging/LogMacros.h"
//bool Socket::Connect()
//{
    //UE_LOG(LogTemp, Warning, TEXT("Socket::Connect() -----  1113333311 -----"));
    //asio::io_service service;
    //asio::ip::tcp::endpoint ep(asio::ip::address::from_string("172.26.25.15"), 2007);
    //asio::ip::tcp::socket sock(service);
    //sock.connect(ep);
    //return true;
//}

#include "TcpSocket.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "Async/Async.h"
#include "Networking.h"

FTCPSocket::FTCPSocket( const FTCPSocketConnectDelegate& ConnectDelegate, 
                        const FTCPSocketDisconnectedDelegate& DisconnectedDelegate, 
                        const FTCPSocketReceivedMessageDelegate& ReceivedMessageDelegate)
    : SocketThreadPtr(nullptr)
    , SocketRunnablePtr(nullptr)
    , ConnectDelegate(ConnectDelegate)
    , DisconnectedDelegate(DisconnectedDelegate)
    , ReceivedMessageDelegate(ReceivedMessageDelegate)
{
}

FTCPSocket::~FTCPSocket()
{
    Disconnect();
}

void FTCPSocket::Connect(const FString& IP, int32 Port)
{
    if (!SocketRunnablePtr.IsValid())
    {
        SendBuffer.Empty();
        RecvData.Empty();
        SocketRunnablePtr = MakeShareable<FTCPSocketRunnable>(new FTCPSocketRunnable(IP, Port, SendBuffer, RecvData));
        SocketRunnablePtr->GetConnectedDelegate().BindLambda([this]()
        {
            AsyncTask(ENamedThreads::GameThread, [this]()
            {
                ConnectDelegate.ExecuteIfBound(true);
            });
        });
        SocketRunnablePtr->GetNewDataReceivedDelegate().BindLambda([this]()
        {
            ReceivedMessageDelegate.ExecuteIfBound(RecvData);
        });
        SocketRunnablePtr->GetConnectionErrorDelegate().BindLambda([this]()
        {
            AsyncTask(ENamedThreads::GameThread, [this]()
            {
                Disconnect();
            });
        });
        SocketThreadPtr = MakeShareable<FRunnableThread>(FRunnableThread::Create(SocketRunnablePtr.Get(), 
                                                                                 *FString::Printf(TEXT("TCPSocket_Thread_%s:%d"), 
                                                                                 *IP, 
                                                                                 Port)));
    }
}

void FTCPSocket::Disconnect()
{
    if (SocketRunnablePtr.IsValid())
    {
        //SocketRunnablePtr->OnConnected().Unbind();
        //SocketRunnablePtr->OnNewDataReceived().Unbind();
        //SocketRunnablePtr->OnConnectionError().Unbind();

        SocketRunnablePtr->Stop();
        SocketThreadPtr->WaitForCompletion();
        SocketThreadPtr.Reset();
        SocketRunnablePtr.Reset();

        DisconnectedDelegate.ExecuteIfBound();
    }
}

void FTCPSocket::Send(uint8* Data, int32 Count)
{
    if (SocketRunnablePtr.IsValid())
    {
        TArray<uint8> ToSend;
        ToSend.Append(Data, Count);
        SendBuffer.Enqueue(ToSend);
    }
}

