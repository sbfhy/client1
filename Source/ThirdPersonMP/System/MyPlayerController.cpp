// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "System/Subsystem/MgrMessage.h"
#include "service/c2g_scene.pb.h"
#include "Base/Define/DefinePos.h"

void AMyPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    if (isNeedSyncMove())
    {
        syncMove();
    }
}

bool AMyPlayerController::isNeedSyncMove()
{
    FRotator nowRotation = GetControlRotation();
    if (nowRotation != m_lastSyncRotation)
        return true;

    FVector nowRocation = GetFocalLocation();
    if (FVector::DistSquared(m_lastSyncLocation, nowRocation) > 1)
        return true;

    ACharacter* m_character = GetCharacter();
    if (!m_character) return false;
    UCharacterMovementComponent* m_movement = m_character->GetCharacterMovement();
    if (!m_movement) return false;
    float nowSpeed = m_movement->GetMaxSpeed();
    if (nowSpeed != m_lastSyncSpeed)
        return true;

    return false;
}

void AMyPlayerController::syncMove()
{
    m_lastSyncRotation = GetControlRotation();
    m_lastSyncLocation = GetFocalLocation();
    ACharacter* m_character = GetCharacter();
    if (!m_character) return ;
    UCharacterMovementComponent* m_movement = m_character->GetCharacterMovement();
    if (!m_movement) return ;
    m_lastSyncSpeed = m_movement->GetMaxSpeed();

    UMgrMessage* pMgrMessage = GetGameInstance()->GetSubsystem<UMgrMessage>();
    if (pMgrMessage)
    {
        CMD::C2G_MoveStateArgPtr C2G_MoveStateArgPtr = std::make_shared<CMD::C2G_MoveStateArg>();
        CMD::MoveStateCmd* cmd = C2G_MoveStateArgPtr->mutable_cmd();
        cmd->set_rotate(FRotator2Rotate(m_lastSyncRotation));
        cmd->set_speed(m_lastSyncSpeed);
        cmd->set_guid(pMgrMessage->GetAccid()); // FIXME guid暂时用accid
        cmd->mutable_curpos()->CopyFrom(FVector2CMDPos(m_lastSyncLocation));
        pMgrMessage->Send(C2G_MoveStateArgPtr);
    }
}

