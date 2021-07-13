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
//#pragma once
//
//#include <ProtobufCodecLite.h>
//
//namespace muduo
//{
//namespace net
//{
//
////typedef std::shared_ptr<class TcpConnection> TcpConnectionPtr;
//
//typedef std::shared_ptr<class RpcMessage> RpcMessagePtr;
//extern const char rpctag[5];// = "RPC0";
//
//// wire format
////
//// Field     Length  Content
////
//// size      4-byte  N+8
//// "RPC0"    4-byte
//// payload   N-byte
//// checksum  4-byte  adler32 of "RPC0"+payload
////
//
//typedef ProtobufCodecLiteT<RpcMessage, rpctag> RpcCodec;
//
//}  // namespace net
//}  // namespace muduo
//
//
