// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "TestUI.generated.h"

/**
 *
 */
UCLASS()
class THIRDPERSONMP_API UTestUI : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
        void onButtonTest();

private:
    UPROPERTY(meta = (BindWidget))
        class UButton* m_ButtonTest{nullptr};
    UPROPERTY(meta = (BindWidget))
        class UImage* m_ImageTest{ nullptr };
    UPROPERTY(meta = (BindWidget))
        class UEditableTextBox* m_EditableTextBox{ nullptr };
};
