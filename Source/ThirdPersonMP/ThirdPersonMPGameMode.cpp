// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonMPGameMode.h"
#include "ThirdPersonMPCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Base/Log/Logger.h"
#include "ThirdPersonMP/System/Managers.h"
#include "ThirdPersonMP/Base/Define/DefineNew.h"

AThirdPersonMPGameMode::AThirdPersonMPGameMode()
{
    // set default pawn class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
    PrimaryActorTick.bCanEverTick = true;
}

AThirdPersonMPGameMode::~AThirdPersonMPGameMode()
{
    //DELETE_SAFE(m_managers);
}

void AThirdPersonMPGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    UE_LOG(LogTemp, Warning, TEXT("AThirdPersonMPGameMode::InitGame()"));
    Super::InitGame(MapName, Options, ErrorMessage);
    //m_managers = new Managers();
}

void AThirdPersonMPGameMode::StartPlay()
{
    //UE_LOG(LogTemp, Warning, TEXT("AThirdPersonMPGameMode::StartPlay()"));
    Super::StartPlay();
    //if (m_managers)
    //{
    //    m_managers->Init();
    //}
}

void AThirdPersonMPGameMode::BeginPlay()
{
    //UE_LOG(LogTemp, Warning, TEXT("AThirdPersonMPGameMode::BeginPlay()"));
    LLOG_NET("AThirdPersonMPGameMode::BeginPlay()");
    Super::BeginPlay();
}

void AThirdPersonMPGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    //UE_LOG(LogTemp, Warning, TEXT("AThirdPersonMPGameMode::EndPlay(), EndPlayReason:%d"), static_cast<int>(EndPlayReason));
    LLOG_NET("AThirdPersonMPGameMode::EndPlay(), EndPlayReason:%d", static_cast<int>(EndPlayReason));
    //if (m_managers)
    //{
    //    m_managers->End();
    //}
    Super::EndPlay(EndPlayReason);
}

void AThirdPersonMPGameMode::Tick(float DeltaSeconds)
{
    //UE_LOG(LogTemp, Warning, TEXT("AThirdPersonMPGameMode::Tick(), DeltaSeconds:%f"), DeltaSeconds);
    Super::Tick(DeltaSeconds);
    //if (m_managers)
    //{
    //    m_managers->Tick(DeltaSeconds * 1000);
    //}
}

