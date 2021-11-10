// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonMPGameMode.h"
#include "ThirdPersonMPCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Base/Log/Logger.h"
#include "ThirdPersonMP/System/Managers.h"
#include "ThirdPersonMP/Base/Define/DefineNew.h"
#include "Kismet/GameplayStatics.h"

#include "System/Subsystem/MgrMessage.h"
#include "service/c2g_scene.pb.h"

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
}

void AThirdPersonMPGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    UE_LOG(LogTemp, Warning, TEXT("AThirdPersonMPGameMode::InitGame()"));
    Super::InitGame(MapName, Options, ErrorMessage);
}

void AThirdPersonMPGameMode::StartPlay()
{
    Super::StartPlay();
}

void AThirdPersonMPGameMode::BeginPlay()
{
    LLOG_HY("AThirdPersonMPGameMode::BeginPlay()， %p", GetWorld());
    Super::BeginPlay();
    
    C2G_NotifyLoadedScene();
}

void AThirdPersonMPGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    LLOG_HY("AThirdPersonMPGameMode::EndPlay(), EndPlayReason:%d", static_cast<int>(EndPlayReason));
    Super::EndPlay(EndPlayReason);
}

void AThirdPersonMPGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AThirdPersonMPGameMode::ChangeApawn(APawn* pPawn)
{
    APlayerController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (pController)
    {
        pController->UnPossess();
        pController->Possess(pPawn);
    }
}

void AThirdPersonMPGameMode::C2G_NotifyLoadedScene()
{
    UMgrMessage* pMgrMessage = GetGameInstance()->GetSubsystem<UMgrMessage>();
    if (pMgrMessage)
    {
        CMD::C2G_NotifyLoadedSceneArgPtr C2G_NotifyLoadedSceneArgPtr = std::make_shared<CMD::C2G_NotifyLoadedSceneArg>();
        C2G_NotifyLoadedSceneArgPtr->set_mapid(1);
        pMgrMessage->Send(C2G_NotifyLoadedSceneArgPtr);
    }
}
