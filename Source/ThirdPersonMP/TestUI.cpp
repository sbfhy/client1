// Fill out your copyright notice in the Description page of Project Settings.

#include "TestUI.h"

#include "Components/Widget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Base/Log/Logger.h"
#include "System/Subsystem/MgrMessage.h"
#include <pb/sudoku.pb.h>

void UTestUI::NativeConstruct()
{
    Super::NativeConstruct();
    if (m_UButtonTest != nullptr)
    {
        m_UButtonTest->OnClicked.AddDynamic(this, &UTestUI::onButtonTest);
    }
    LLOG_UI("");
}

void UTestUI::onButtonTest()
{
    LLOG_UI("点击按钮");
    if (m_UImageTest->GetVisibility() == ESlateVisibility::Hidden)
    {
        m_UImageTest->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        m_UImageTest->SetVisibility(ESlateVisibility::Hidden);
    }

    UMgrMessage* mgrMessage = GetGameInstance()->GetSubsystem<UMgrMessage>();
    if (mgrMessage)
    {
        sudoku::SudokuRequest request;
        request.set_checkerboard("001010");
        sudoku::SudokuResponse* response = new sudoku::SudokuResponse;
        auto stub = static_cast<sudoku::SudokuService_Stub*>(mgrMessage->GetRpcService().GetStub("sudokuStub"));
        if (stub)
        {
            LLOG_UI("调用SudokuService_Stub的Solve(), mgrAddr:%p", mgrMessage);
            stub->Solve(nullptr, &request, response, NewCallback(this, &UTestUI::solved, response));
        }
    }
}

void UTestUI::solved(sudoku::SudokuResponse* response)
{
    LLOG_UI("%d %s", response->solved(), *FString(response->checkerboard().c_str()));
}


