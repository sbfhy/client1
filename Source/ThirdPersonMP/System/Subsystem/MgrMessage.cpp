// 协议格式
// --------------------------------------------
// |Length(4)|tag(M)|Payload(N-2)|checksum(4)|
// --------------------------------------------
// size      4-byte  M+N+4
// tag       M-byte  could be "RPC0", etc.
// payload   N-byte
// checksum  4-byte  adler32 of tag+payload

#include "MgrMessage.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "Base/Log/Logger.h"
#include "Network/muduo/RpcChannel.h"

const float FIXED_DELTA_TIME = 1.0f;
const char rpctag[] = "RPC0";

UMgrMessage::UMgrMessage()
    : m_RpcChannelPtr(MakeShareable(new RpcChannel(this)))
    , m_RpcService(m_RpcChannelPtr)
{
    LLOG_NET("%p", this);
    InternalConnectFunc = [](const FString&) {};
    InternalDisconnectFunc = []() {};
    InternalSendPbcFunc = [](const std::string&) {};
}

void UMgrMessage::Connect(const FString& Host)
{
    LLOG_NET("%s", *Host);
    InternalConnectFunc(Host);
}

void UMgrMessage::Disconnect()
{
    LLOG_NET("");
    InternalDisconnectFunc();
}

void UMgrMessage::TickMessageQueue(float DeltaTime)
{
    TArray<uint8> Content;

    //while (OnServerReceiveDelegate.IsBound() && MessageQueue.Dequeue(Content))
    //{
    //    OnServerReceiveDelegate.ExecuteIfBound(Content);
    //}
    while (m_RpcChannelPtr && MessageQueue.Dequeue(Content))
    {
        m_RpcChannelPtr->OnMessage(Content);
    }
}

void UMgrMessage::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    ConnectDelegate.BindLambda([this](bool Success) {
        if (Success)
        {
            OnServerConnectedDelegate.ExecuteIfBound();
        }
        else
        {
            OnServerDisconnectedDelegate.ExecuteIfBound();
        }
    });

    DisconnectedDelegate.BindLambda([this]() {
        OnServerDisconnectedDelegate.ExecuteIfBound();
    });

    ReceivedMessageDelegate.BindLambda([this](TArray<uint8>& DataBuffer) {
        uint32 Size = DataBuffer.Num();
        LLOG_NET("len:%u, %s", Size, *FString(std::string(reinterpret_cast<const char*>(DataBuffer.GetData()), DataBuffer.Num()).c_str()));
        while (DataBuffer.Num() > 4)
        {
            auto Length = *(uint32*)(&DataBuffer[0]);
            if (Size < kHeaderLen + Length)
            {
                return;
            }
            uint8* Data = DataBuffer.GetData();
            TArray<uint8> toData(Data + kHeaderLen, Length);
            // FIXME 字节序, checksum  
            //uint32_t checksum = *(uint32_t*)(Data + Length - 4);
            DataBuffer.RemoveAt(0, kHeaderLen + Length, false);
            Size = DataBuffer.Num();
            MessageQueue.Enqueue(MoveTemp(toData));
        }
    });

    InternalConnectFunc = [this](const FString& Host) {
        if (!MessageDelegateHandler.IsValid())
        {
            MessageDelegateHandler = GetWorld()->OnTickDispatch().AddUObject(this, &UMgrMessage::TickMessageQueue);
        }

        int Index = -1;
        Host.FindLastChar(TEXT(':'), Index);
        FString IP = Host.Left(Index);
        FString Port = Host.Right(Host.Len() - Index - 1);

        TCPSocketPtr->Connect(IP, FCString::Atoi(*Port));
    };
    InternalDisconnectFunc = [this]() {
        TCPSocketPtr->Disconnect();

        if (MessageDelegateHandler.IsValid())
        {
            GetWorld()->OnTickDispatch().Remove(MessageDelegateHandler);
            MessageDelegateHandler.Reset();
        }

        MessageQueue.Empty();
    };
    InternalSendPbcFunc = [this](const std::string& msg) {
        //std::string tagPayload(rpctag);
        //tagPayload.append(msg);
        //uint32_t checksum = static_cast<uint32_t>(::adler32(1, static_cast<const Bytef*>(tagPayload.data()), tagPayload.size()));
        uint32_t length = msg.size();
        // FIXME 字节序,checksum

        std::string tosend;
        tosend.append((char*)&length, 4);
        tosend.append(msg);
        //tosend.append((char*)&checksum, 4);
        TCPSocketPtr->Send((uint8*)tosend.data(), tosend.size());
        /*TCPSocketPtr->Send((uint8*)&header, 4);
        TCPSocketPtr->Send((uint8*)msg.data(), msg.size());*/
    };

    TCPSocketPtr = MakeShareable(new FTCPSocket(ConnectDelegate, DisconnectedDelegate, ReceivedMessageDelegate));
    m_RpcChannelPtr->SetTCPSocketPtr(TCPSocketPtr);
    
    Connect(FString("172.26.25.15:9981"));
}

void UMgrMessage::Deinitialize()
{
    Disconnect();

    InternalConnectFunc = [](const FString&) {};
    InternalDisconnectFunc = []() {};
    InternalSendPbcFunc = [](const std::string&) {};

    Super::Deinitialize();
}

void UMgrMessage::SendMessage(const std::string& msg)
{
    LLOG_NET(" %s, size:%u", *FString(msg.c_str()), msg.size());
    InternalSendPbcFunc(msg);
}