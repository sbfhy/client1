#include "RpcService.h"

#include "Network/muduo/RpcChannel.h"
#include "pb/sudoku.pb.h"
#include "Network/service/c2g_userloginservice.h"

#define  ServiceRegister(NAME, TYPE)                                        \
    do {                                                                    \
        ::google::protobuf::Service* NAME = new TYPE(pRpcChannel.Get());    \
        m_MapStub[#NAME] = NAME;                                            \
    } while(false)
   

void RpcService::RpcServiceRegStub(TSharedPtr<class RpcChannel> pRpcChannel)
{
    //::google::protobuf::Service* sudokuStub = new sudoku::SudokuService_Stub(pRpcChannel.Get());
    //m_MapStub["sudokuStub"] = sudokuStub;

    //ServiceRegister(SudokuService_Stub, sudoku::SudokuService_Stub);
    ServiceRegister(C2G_UserLoginService_Stub, RPC::C2G_UserLoginService_Stub);

}

