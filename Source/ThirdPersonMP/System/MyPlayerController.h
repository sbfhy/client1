// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 *
 */
UCLASS()
class THIRDPERSONMP_API AMyPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
	//~ Begin APlayerController Interface

    /**
     * Processes player input (immediately after PlayerInput gets ticked) and calls UpdateRotation().
     * PlayerTick is only called if the PlayerController has a PlayerInput object. Therefore, it will only be called for locally controlled PlayerControllers.
     */
    virtual void PlayerTick(float DeltaTime) override;  // 只有拥有PlayerInput的才会调用
	//~ End APlayerController Interface

private:
    bool isNeedSyncMove();                              // 是否需要给后端同步一下移动信息
    void syncMove();
    FVector     m_lastSyncLocation;
    FRotator    m_lastSyncRotation;
    float       m_lastSyncSpeed{ 0 };
};
