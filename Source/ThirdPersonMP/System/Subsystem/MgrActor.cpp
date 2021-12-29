// Fill out your copyright notice in the Description page of Project Settings.


#include "MgrActor.h"

#include "ThirdPersonMP/ThirdPersonMPCharacter.h"
#include "UObject/UObjectGlobals.h"


APawn* UMgrActor::SpawnActor()
{
    UWorld* pWorld = GetWorld();
    if (!pWorld) return nullptr;
    UClass* pClass = LoadClass<ACharacter>(nullptr, TEXT("Blueprint'/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter.ThirdPersonCharacter_C'"));
    APawn* pPawn = pWorld->SpawnActor<APawn>(pClass, FVector(0, 0, 500), FRotator(0, 0, 0));
    return pPawn;
}
