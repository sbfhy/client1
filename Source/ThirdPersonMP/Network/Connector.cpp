//#include "Connector.h"
//#include "ThirdPersonMP/Base/Log/Logger.h"
//#include <functional>
//
//void Connector::Start()
//{
//    UE_LOG(NetLog, Log, TEXT("Connector::Start()"));
//    m_isStarted = true;
//    connect();
//}
//
//void Connector::Stop()
//{
//    if (!m_isStarted) return;
//    UE_LOG(NetLog, Warning, TEXT("Connector::Stop() "));
//    m_isStarted = false;
//    if (getSocket())
//    {
//        getSocket()->shutdown(asio::ip::tcp::socket::shutdown_both);
//        getSocket()->cancel();
//        getSocket()->close();
//    }
//}
//
//void Connector::connect()
//{
//    if (m_Socket)
//    {
//        getSocket()->async_connect( m_Socket->m_endpoint, 
//                                    std::bind(&SelfType::onConnect, shared_from_this(), std::placeholders::_1) );
//    }
//}
//
//void Connector::onConnect(const asio::error_code& err)
//{
//    if (!err)
//    {
//        UE_LOG(NetLog, Log, TEXT("Connector::onConnect() success"));
//    }
//    else
//    {
//        UE_LOG(NetLog, Error, TEXT("Connector::onConnect() failed"));
//        Stop();
//    }
//}
//
//void Connector::disConnect()
//{
//
//}
//
