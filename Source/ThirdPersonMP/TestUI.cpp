// Fill out your copyright notice in the Description page of Project Settings.

#include "TestUI.h"

#include "Components/Widget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/EditableTextBox.h"
#include "Base/Log/Logger.h"
#include "System/Subsystem/MgrMessage.h"

#include "service/c2a_user.pb.h"

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

    UMgrMessage* pMgrMessage = GetGameInstance()->GetSubsystem<UMgrMessage>();
    if (pMgrMessage)
    {
        CMD::C2A_UserSignInArgPtr C2A_UserSignInArgPtr = std::make_shared<CMD::C2A_UserSignInArg>();
        uint64_t accid = FCString::Strtoui64(*m_EditableTextBox->GetText().ToString(), nullptr, 10);
        // TODO 账号格式检查
        C2A_UserSignInArgPtr->set_accid(accid);
        pMgrMessage->SetAccid(accid);
        pMgrMessage->Send(C2A_UserSignInArgPtr);
    }
    
    
    SetVisibility(ESlateVisibility::Hidden);
}

