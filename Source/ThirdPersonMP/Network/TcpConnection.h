//#pragma once
//
//#include "ThirdPersonMP/Base/Common/Noncopyable.h"
//#include "ThirdPersonMP/Base/Define/DefineVariable.h"
//#include "ThirdPersonMP/Network/muduo/StringPiece.h"
//#include "ThirdPersonMP/Network/muduo/Callbacks.h"
//#include "ThirdPersonMP/Network/muduo/Buffer.h"
//#include <memory>
//#include <thread>
//#include <mutex>
//
//namespace muduo
//{
//namespace net
//{
//
//class TcpConnection : public Noncopyable
//{
//public:
//    virtual ~TcpConnection();
//    void Init();
//    void Send(const void* message, SDWORD len);
//    void Send(const StringPiece& message);
//    void Send(Buffer* message);
//    void Shutdown();
//
//    bool IsConnected() const { return m_state == kConnected; }
//    bool IsDisconnected() const { return m_state == kDisconnecting; }
//
//private:
//    void start();
//    void stop();
//    enum EState { kDisconnected, kConnecting, kConnected, kDisconnecting };
//    void setState(EState state) { m_state = state; }
//
//    void doDisconnect(const std::error_code& ec);
//
//private:
//    std::shared_ptr<class Connector>    m_connector;
//    std::shared_ptr<class Socket>       m_socket;
//    std::unique_ptr<std::thread>        m_thread;
//    std::mutex                          m_mutex;
//    Buffer                              m_inputBuffer;
//    Buffer                              m_outputBuffer;
//    std::unique_ptr<class RpcChannel>   m_rpcChannel;
//    EState                              m_state;                // FIXME: use atomic variable
//};
//
//typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
//
//}       // namespace net
//}       // namespace muduo
//
//
