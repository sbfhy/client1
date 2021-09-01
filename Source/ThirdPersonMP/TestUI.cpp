// Fill out your copyright notice in the Description page of Project Settings.

#include "TestUI.h"

#include "Components/Widget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/EditableTextBox.h"
#include "Base/Log/Logger.h"
#include "System/Subsystem/MgrMessage.h"
#include "service/c2g_user.pb.h"
#include "Network/service/c2g_userloginservice.h"

void UTestUI::NativeConstruct()
{
    Super::NativeConstruct();
    if (m_ButtonTest != nullptr)
    {
        m_ButtonTest->OnClicked.AddDynamic(this, &UTestUI::onButtonTest);
    }
}

void UTestUI::onButtonTest()
{
    if (!m_ImageTest || !m_EditableTextBox) return;

    LLOG_UI("%s", *m_EditableTextBox->GetText().ToString());

    UMgrMessage* mgrMessage = GetGameInstance()->GetSubsystem<UMgrMessage>();
    if (mgrMessage)
    {
        std::shared_ptr<CMD::C2G_UserLoginArg> request = std::make_shared<CMD::C2G_UserLoginArg>();
        uint64_t accid = FCString::Strtoui64(*m_EditableTextBox->GetText().ToString(), nullptr, 10);
        request->set_accid(accid);
        mgrMessage->Send(request);
    }
    
    SetVisibility(ESlateVisibility::Hidden);
}

//void UTestUI::solved(sudoku::SudokuResponse* response)
//{
//    LLOG_UI("%d %s", response->solved(), *FString(response->checkerboard().c_str()));
//}

