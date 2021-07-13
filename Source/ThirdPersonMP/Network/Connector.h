//#pragma once
//
//#include "ThirdPersonMP/Base/Common/Noncopyable.h"
//#include "Socket.h"
//#include <memory>
//
//class Connector : private Noncopyable
//                , public std::enable_shared_from_this<Connector>
//{
//public:
//    typedef Connector SelfType;
//
//    Connector(std::shared_ptr<class Socket> socket_) : m_Socket(socket_) {}
//
//    void Start();
//    void Stop();
//
//private:
//    void connect();
//    void onConnect(const asio::error_code& err);
//    void disConnect();
//    asio::ip::tcp::socket* getSocket() { if (m_Socket) return &m_Socket->m_socket; return nullptr; }
//    asio::io_service* getService() { if (m_Socket) return &m_Socket->m_service; return nullptr; }
//
//private:
//    std::shared_ptr<class Socket> m_Socket;
//    bool m_isStarted;
//};
//
