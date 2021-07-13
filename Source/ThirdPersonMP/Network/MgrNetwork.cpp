//#include "MgrNetwork.h"
//
//#include "ThirdPersonMP/Base/Log/Logger.h"
//#include "ThirdPersonMP/Base/Time/Timestamp.h"
//#include "TcpConnection.h"
////#include "pb/sudoku.pb.h"
////#include "google/protobuf/util/json_util.h"
//
//MgrNetwork::MgrNetwork()
//{
//    m_connection = std::make_shared<TcpConnection>();
//}
//
//MgrNetwork::~MgrNetwork()
//{
//}
//
//bool MgrNetwork::Init()
//{
//    //UE_LOG(LogTemp, Error, TEXT("%s, MgrNetwork::Init()"), UTF8_TO_TCHAR(Timestamp::GetTimeNow()) );
//    UE_LOG(NetLog, Log, TEXT("MgrNetwork::Init()") );
//    if (m_connection)
//    {
//        m_connection->Init();
//    }
//
//    //sudoku::SudokuRequest testRequest;
//    //sudoku::SudokuResponse testService;
//    //google::protobuf::util::JsonParseOptions;
//
//    return true;
//}
//
//void MgrNetwork::Tick(UDWORD deltaMSec)
//{
//    UE_LOG(MgrTick, Log, TEXT("%s, MgrNetwork::Tick, deltaMSec:%u"), Timestamp::GetTimeNow(), deltaMSec);
//}
//
