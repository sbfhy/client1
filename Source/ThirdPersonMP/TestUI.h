// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "TestUI.generated.h"

namespace sudoku {
    class SudokuResponse;
}

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

    void solved(sudoku::SudokuResponse* response);

private:
    UPROPERTY(meta = (BindWidget))
        class UButton* m_UButtonTest;
    UPROPERTY(meta = (BindWidget))
        class UImage* m_UImageTest;
};
