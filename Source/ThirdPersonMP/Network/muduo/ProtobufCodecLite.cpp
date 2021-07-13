//// Copyright 2010, Shuo Chen.  All rights reserved.
//// http://code.google.com/p/muduo/
////
//// Use of this source code is governed by a BSD-style license
//// that can be found in the License file.
//
//// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
//#include "ProtobufCodecLite.h"
//#include "Buffer"
//#include "ThirdPersonMP/Network/TcpConnection.h"
//#include "google_inl.h"
//#include "ThirdPersonMP/Base/Log/Logger.h"
//#include <google/protobuf/message.h>
//#include <WinSock2.h>           // endian
////#include <zlib.h>
//
//using namespace muduo;
//using namespace muduo::net;
//
//namespace
//{
//    int ProtobufVersionCheck()
//    {
//        GOOGLE_PROTOBUF_VERIFY_VERSION;
//        return 0;
//    }
//    int __attribute__((unused)) dummy = ProtobufVersionCheck();
//}
//
//void ProtobufCodecLite::Send(const TcpConnectionPtr& conn,
//                             const ::google::protobuf::Message& message)
//{
//    // FIXME: serialize to TcpConnection::outputBuffer()
//    muduo::net::Buffer buf;
//    FillEmptyBuffer(&buf, message);
//    conn->Send(&buf);
//}
//
//void ProtobufCodecLite::FillEmptyBuffer(muduo::net::Buffer* buf,
//                                        const google::protobuf::Message& message)
//{
//    assert(buf->readableBytes() == 0);
//    // FIXME: can we move serialization & checksum to other thread?
//    buf->append(tag_);
//
//    int byte_size = SerializeToBuffer(message, buf);
//
//    int32_t checkSum = Checksum(buf->peek(), static_cast<int>(buf->readableBytes()));
//    buf->appendInt32(checkSum);
//    assert(buf->readableBytes() == tag_.size() + byte_size + kChecksumLen); (void)byte_size;
//    int32_t len = htonl(static_cast<int32_t>(buf->readableBytes()));
//    buf->prepend(&len, sizeof len);
//}
//
//void ProtobufCodecLite::OnMessage(const TcpConnectionPtr& conn,
//                                  Buffer* buf,
//                                  Timestamp receiveTime)
//{
//    while (buf->readableBytes() >= static_cast<uint32_t>(kMinMessageLen + kHeaderLen))
//    {
//        const int32_t len = buf->peekInt32();
//        if (len > kMaxMessageLen || len < kMinMessageLen)
//        {
//            errorCallback_(conn, buf, receiveTime, kInvalidLength);
//            break;
//        }
//        else if (buf->readableBytes() >= implicit_cast<size_t>(kHeaderLen + len))
//        {
//            if (rawCb_ && !rawCb_(conn, StringPiece(buf->peek(), kHeaderLen + len), receiveTime))
//            {
//                buf->retrieve(kHeaderLen + len);
//                continue;
//            }
//            MessagePtr message(prototype_->New());
//            // FIXME: can we move deserialization & callback to other thread?
//            ErrorCode errorCode = Parse(buf->peek() + kHeaderLen, len, message.get());
//            if (errorCode == kNoError)
//            {
//                // FIXME: try { } catch (...) { }
//                messageCallback_(conn, message, receiveTime);
//                buf->retrieve(kHeaderLen + len);
//            }
//            else
//            {
//                errorCallback_(conn, buf, receiveTime, errorCode);
//                break;
//            }
//        }
//        else
//        {
//            break;
//        }
//    }
//}
//
//bool ProtobufCodecLite::ParseFromBuffer(StringPiece buf, google::protobuf::Message* message)
//{
//    return message->ParseFromArray(buf.data(), static_cast<int>(buf.size()));
//}
//
//int ProtobufCodecLite::SerializeToBuffer(const google::protobuf::Message& message, Buffer* buf)
//{
//    // TODO: use BufferOutputStream
//    // BufferOutputStream os(buf);
//    // message.SerializeToZeroCopyStream(&os);
//    // return static_cast<int>(os.ByteCount());
//
//    // code copied from MessageLite::SerializeToArray() and MessageLite::SerializePartialToArray().
//    GOOGLE_DCHECK(message.IsInitialized()) << InitializationErrorMessage("serialize", message);
//
//    /**
//     * 'ByteSize()' of message is deprecated in Protocol Buffers v3.4.0 firstly. But, till to v3.11.0, it just getting start to be marked by '__attribute__((deprecated()))'.
//     * So, here, v3.9.2 is selected as maximum version using 'ByteSize()' to avoid potential effect for previous muduo code/projects as far as possible.
//     * Note: All information above just INFER from
//     * 1) https://github.com/protocolbuffers/protobuf/releases/tag/v3.4.0
//     * 2) MACRO in file 'include/google/protobuf/port_def.inc'. eg. '#define PROTOBUF_DEPRECATED_MSG(msg) __attribute__((deprecated(msg)))'.
//     * In addition, usage of 'ToIntSize()' comes from Impl of ByteSize() in new version's Protocol Buffers.
//     */
//
//#if GOOGLE_PROTOBUF_VERSION > 3009002
//    int byte_size = google::protobuf::internal::ToIntSize(message.ByteSizeLong());
//#else
//    int byte_size = message.ByteSize();
//#endif
//    buf->ensureWritableBytes(byte_size + kChecksumLen);
//
//    uint8_t* start = reinterpret_cast<uint8_t*>(buf->beginWrite());
//    uint8_t* end = message.SerializeWithCachedSizesToArray(start);
//    if (end - start != byte_size)
//    {
//#if GOOGLE_PROTOBUF_VERSION > 3009002
//        ByteSizeConsistencyError(byte_size, google::protobuf::internal::ToIntSize(message.ByteSizeLong()), static_cast<int>(end - start));
//#else
//        ByteSizeConsistencyError(byte_size, message.ByteSize(), static_cast<int>(end - start));
//#endif
//    }
//    buf->hasWritten(byte_size);
//    return byte_size;
//}
//
//namespace
//{
//    const string kNoErrorStr = "NoError";
//    const string kInvalidLengthStr = "InvalidLength";
//    const string kCheckSumErrorStr = "CheckSumError";
//    const string kInvalidNameLenStr = "InvalidNameLen";
//    const string kUnknownMessageTypeStr = "UnknownMessageType";
//    const string kParseErrorStr = "ParseError";
//    const string kUnknownErrorStr = "UnknownError";
//}
//
//const string& ProtobufCodecLite::ErrorCodeToString(ErrorCode errorCode)
//{
//    switch (errorCode)
//    {
//    case kNoError:
//        return kNoErrorStr;
//    case kInvalidLength:
//        return kInvalidLengthStr;
//    case kCheckSumError:
//        return kCheckSumErrorStr;
//    case kInvalidNameLen:
//        return kInvalidNameLenStr;
//    case kUnknownMessageType:
//        return kUnknownMessageTypeStr;
//    case kParseError:
//        return kParseErrorStr;
//    default:
//        return kUnknownErrorStr;
//    }
//}
//
//void ProtobufCodecLite::DefaultErrorCallback(const TcpConnectionPtr& conn,
//                                             Buffer* buf,
//                                             Timestamp,
//                                             ErrorCode errorCode)
//{
//    UE_LOG(NetLog, Error, TEXT("ProtobufCodecLite::defaultErrorCallback - %s"), ErrorCodeToString(errorCode).c_str());
//    if (conn && conn->connected())
//    {
//        conn->shutdown();
//    }
//}
//
//int32_t ProtobufCodecLite::AsInt32(const char* buf)
//{
//    int32_t be32 = 0;
//    ::memcpy(&be32, buf, sizeof(be32));
//    return ntohs(be32);
//}
//
//int32_t ProtobufCodecLite::Checksum(const void* buf, int len)
//{
//    return static_cast<int32_t>(
//        ::adler32(1, static_cast<const Bytef*>(buf), len));
//}
//
//bool ProtobufCodecLite::ValidateChecksum(const char* buf, int len)
//{
//    // check sum
//    int32_t expectedCheckSum = AsInt32(buf + len - kChecksumLen);
//    int32_t checkSum = Checksum(buf, len - kChecksumLen);
//    return checkSum == expectedCheckSum;
//}
//
//ProtobufCodecLite::ErrorCode ProtobufCodecLite::Parse(const char* buf,
//    int len,
//    ::google::protobuf::Message* message)
//{
//    ErrorCode error = kNoError;
//
//    if (ValidateChecksum(buf, len))
//    {
//        if (memcmp(buf, tag_.data(), tag_.size()) == 0)
//        {
//            // parse from buffer
//            const char* data = buf + tag_.size();
//            int32_t dataLen = len - kChecksumLen - static_cast<int>(tag_.size());
//            if (ParseFromBuffer(StringPiece(data, dataLen), message))
//            {
//                error = kNoError;
//            }
//            else
//            {
//                error = kParseError;
//            }
//        }
//        else
//        {
//            error = kUnknownMessageType;
//        }
//    }
//    else
//    {
//        error = kCheckSumError;
//    }
//
//    return error;
//}
//
