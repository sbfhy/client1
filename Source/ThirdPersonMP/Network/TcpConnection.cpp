//#include "TcpConnection.h"
//#include "ThirdPersonMP/Network/Connector.h"
//#include "Socket.h"
//#include "ThirdPersonMP/Base/Log/Logger.h"
//#include "AsioHeader.h"
//
//using namespace muduo::net;
//
//TcpConnection::~TcpConnection()
//{
//}
//
//void TcpConnection::Init()
//{
//    UE_LOG(NetLog, Log, TEXT("TcpConnection::Init()"));
//    m_socket = std::make_shared<Socket>();
//    m_connector = std::make_shared<Connector>(m_socket);
//}
//
//void TcpConnection::Send(const void* data, int len)
//{
//    Send(StringPiece(static_cast<const char*>(data), len));
//}
//
//void TcpConnection::Send(const StringPiece& message)
//{
//    if (m_state == kConnected)
//    {
//        if (loop_->isInLoopThread())
//        {
//            sendInLoop(message);
//        }
//        else
//        {
//            void (TcpConnection:: * fp)(const StringPiece & message) = &TcpConnection::sendInLoop;
//            loop_->runInLoop(
//                std::bind(fp,
//                    this,     // FIXME
//                    string(message)));
//            //std::forward<string>(message)));
//        }
//    }
//}
//
//void TcpConnection::Shutdown()
//{
//    if (m_state == kConnected)
//    {
//        setState(kDisconnecting);
//        stop();
//    }
//}
//
//void TcpConnection::start()
//{
//    if (m_socket && m_connector)
//    {
//        m_socket->SetEndpoint(asio::ip::tcp::endpoint(asio::ip::address::from_string("172.26.25.15"), 2007));
//
//        m_thread = std::make_unique<std::thread>(
//            static_cast<std::size_t(asio::io_service::*)()>(&asio::io_service::run),
//            &m_socket->m_service
//        );
//
//        m_connector->Start();
//    }
//}
//
//void TcpConnection::stop()
//{
//    if (m_connector)
//    {
//        m_connector->Stop();
//    }
//    if (m_socket)
//    {
//        m_socket->m_service.stop();
//    }
//    if (m_thread)
//    {
//        m_thread->join();
//    }
//}
//
//void TcpConnection::doDisconnect(const asio::error_code& ec)
//{
//    UE_LOG(NetLog, Log, TEXT("TcpConnection::doDisconnect %s"), ec.message().c_str());
//
//    if (m_socket && m_socket->m_socket->is_open())              // 只有被动关闭才通知
//    {
//        m_socket->m_service.post();                             //
//    }
//}
//
//
