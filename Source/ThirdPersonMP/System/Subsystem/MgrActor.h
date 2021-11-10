// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/Subsystem/MgrBaseGameInstanceSubsystem.h"
#include "MgrActor.generated.h"

/**
 *
 */
UCLASS()
class THIRDPERSONMP_API UMgrActor : public UMgrBaseGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    APawn* SpawnActor();
};
