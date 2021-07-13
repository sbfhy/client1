//// Copyright 2010, Shuo Chen.  All rights reserved.
//// http://code.google.com/p/muduo/
////
//// Use of this source code is governed by a BSD-style license
//// that can be found in the License file.
//
//// Author: Shuo Chen (chenshuo at chenshuo dot com)
////
//// This is a public header file, it must only include public header files.
//
//// For Protobuf codec supporting multiple message types, check
//// examples/protobuf/codec
//
//#pragma once
//
//#include "ThirdPersonMP/Base/Common/Noncopyable.h"
//#include "ThirdPersonMP/Base/Time/Timestamp.h"
//#include "StringPiece.h"
//#include "Callbacks.h"
//
//#include <memory>
//#include <type_traits>
//#include <string>
//
//using std::string;
//
//namespace google
//{
//    namespace protobuf
//    {
//        class Message;
//    }
//}
//
//namespace muduo
//{
//namespace net
//{
//
//typedef std::shared_ptr<google::protobuf::Message> MessagePtr;
//
//// wire format
////
//// Field     Length  Content
////
//// size      4-byte  M+N+4
//// tag       M-byte  could be "RPC0", etc.
//// payload   N-byte
//// checksum  4-byte  adler32 of tag+payload
////
//// This is an internal class, you should use ProtobufCodecT instead.
//class ProtobufCodecLite : Noncopyable
//{
//public:
//    const static int kHeaderLen = sizeof(int32_t);
//    const static int kChecksumLen = sizeof(int32_t);
//    const static int kMaxMessageLen = 64 * 1024 * 1024; // same as codec_stream.h kDefaultTotalBytesLimit
//
//    enum ErrorCode
//    {
//        kNoError = 0,
//        kInvalidLength,
//        kCheckSumError,
//        kInvalidNameLen,
//        kUnknownMessageType,
//        kParseError,
//    };
//
//    // return false to stop parsing protobuf message
//    typedef std::function<bool(const TcpConnectionPtr&, StringPiece, Timestamp)> RawMessageCallback;
//    typedef std::function<void(const TcpConnectionPtr&, const MessagePtr&, Timestamp)> ProtobufMessageCallback;
//    typedef std::function<void(const TcpConnectionPtr&, Buffer*, Timestamp, ErrorCode)> ErrorCallback;
//
//    ProtobufCodecLite(const ::google::protobuf::Message* prototype,
//                      StringPiece tagArg,
//                      const ProtobufMessageCallback& messageCb,
//                      const RawMessageCallback& rawCb = RawMessageCallback(),
//                      const ErrorCallback& errorCb = DefaultErrorCallback)
//        : prototype_(prototype)
//        , tag_(tagArg)
//        , messageCallback_(messageCb)
//        , rawCb_(rawCb)
//        , errorCallback_(errorCb)
//        , kMinMessageLen(static_cast<int>(tagArg.size() + kChecksumLen))
//    {
//    }
//
//    virtual ~ProtobufCodecLite() = default;
//
//    const string& GetTag() const { return tag_; }
//
//    void Send(const TcpConnectionPtr& conn,
//              const ::google::protobuf::Message& message);
//
//    void OnMessage(const TcpConnectionPtr& conn,
//                   Buffer* buf,
//                   Timestamp receiveTime);
//
//    virtual bool ParseFromBuffer(StringPiece buf, google::protobuf::Message* message);
//    virtual int SerializeToBuffer(const google::protobuf::Message& message, Buffer* buf);
//
//    static const string& ErrorCodeToString(ErrorCode errorCode);
//
//    // public for unit tests
//    ErrorCode Parse(const char* buf, int len, ::google::protobuf::Message* message);
//    void FillEmptyBuffer(muduo::net::Buffer* buf, const google::protobuf::Message& message);
//
//    static int32_t Checksum(const void* buf, int len);
//    static bool ValidateChecksum(const char* buf, int len);
//    static int32_t AsInt32(const char* buf);
//    static void DefaultErrorCallback(const TcpConnectionPtr&,
//                                     Buffer*,
//                                     Timestamp,
//                                     ErrorCode);
//
//private:
//    const ::google::protobuf::Message* prototype_;
//    const string tag_;
//    ProtobufMessageCallback messageCallback_;
//    RawMessageCallback rawCb_;
//    ErrorCallback errorCallback_;
//    const int kMinMessageLen;
//};
//
//template<typename MSG, const char* TAG, typename CODEC = ProtobufCodecLite>  // TAG must be a variable with external linkage, not a string literal
//class ProtobufCodecLiteT
//{
//    static_assert(std::is_base_of<ProtobufCodecLite, CODEC>::value, "CODEC should be derived from ProtobufCodecLite");
//public:
//    typedef std::shared_ptr<MSG> ConcreteMessagePtr;
//    typedef std::function<void(const TcpConnectionPtr&, const ConcreteMessagePtr&, Timestamp)> ProtobufMessageCallback;
//    typedef ProtobufCodecLite::RawMessageCallback RawMessageCallback;
//    typedef ProtobufCodecLite::ErrorCallback ErrorCallback;
//
//    explicit ProtobufCodecLiteT(const ProtobufMessageCallback& messageCb,
//                                const RawMessageCallback& rawCb = RawMessageCallback(),
//                                const ErrorCallback& errorCb = ProtobufCodecLite::defaultErrorCallback)
//        : messageCallback_(messageCb)
//        , codec_(&MSG::default_instance(),
//                 TAG,
//                 std::bind(&ProtobufCodecLiteT::onRpcMessage, this, _1, _2, _3),
//                 rawCb,
//                 errorCb)
//    {
//    }
//
//    const string& GetTag() const { return codec_.GetTag(); }
//
//    void Send(const TcpConnectionPtr& conn,
//              const MSG& message)
//    {
//        codec_.Send(conn, message);
//    }
//
//    void OnMessage(const TcpConnectionPtr& conn,
//                   Buffer* buf,
//                   Timestamp receiveTime)
//    {
//        codec_.OnMessage(conn, buf, receiveTime);
//    }
//
//    // internal
//    void OnRpcMessage(const TcpConnectionPtr& conn,
//                      const MessagePtr& message,
//                      Timestamp receiveTime)
//    {
//        messageCallback_(conn, ::muduo::down_pointer_cast<MSG>(message), receiveTime);
//    }
//
//    void FillEmptyBuffer(muduo::net::Buffer* buf, const MSG& message)
//    {
//        codec_.FillEmptyBuffer(buf, message);
//    }
//
//private:
//    ProtobufMessageCallback messageCallback_;
//    CODEC codec_;
//};
//
//}  // namespace net
//}  // namespace muduo
//
