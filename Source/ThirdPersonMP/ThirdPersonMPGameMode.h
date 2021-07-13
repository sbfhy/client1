// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ThirdPersonMP/System/Managers.h"

#include "ThirdPersonMPGameMode.generated.h"

UCLASS(minimalapi)
class AThirdPersonMPGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AThirdPersonMPGameMode();
    virtual ~AThirdPersonMPGameMode();

    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

    virtual void StartPlay() override;

    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void Tick(float DeltaSeconds) override;

private:
    //class Managers* m_managers = nullptr;
};


