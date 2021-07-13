
#include "RpcService.h"
#include "Network/muduo/RpcChannel.h"
#include <pb/sudoku.pb.h>

#define  ServiceRegister(NAME, TYPE, MAP)                                   \
    do {                                                                    \
        ::google::protobuf::Service* NAME = new TYPE(pRpcChannel.Get());    \
        MAP[#NAME] = NAME;                                                  \
    } while(false)
   

void RpcService::RpcServiceRegStub(TSharedPtr<class RpcChannel> pRpcChannel)
{
    //::google::protobuf::Service* sudokuStub = new sudoku::SudokuService_Stub(pRpcChannel.Get());
    //m_MapStub["sudokuStub"] = sudokuStub;

    ServiceRegister(sudokuStub, sudoku::SudokuService_Stub, m_MapStub);
}

