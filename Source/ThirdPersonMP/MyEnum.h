// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MyEnum.generated.h"

/**
 *
 */
UENUM()
enum class MyEnum : uint8
{
    VE_1 = 0   UMETA(DisplayName = "1"),
    VE_2 = 10  UMETA(DisplayName = "2"),
    VE_3 = 20  UMETA(DisplayName = "3")
};
