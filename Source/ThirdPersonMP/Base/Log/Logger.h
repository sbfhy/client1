#pragma once

#include "Logging/LogMacros.h"

//Current Class Name + Function Name where this is called!
#define JOYSTR_CUR_CLASS_FUNC (FString(__FUNCTION__))

//Current Class where this is called!
#define JOYSTR_CUR_CLASS (FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))) )

//Current Function Name where this is called!
#define JOYSTR_CUR_FUNC (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2 ))

//Current Line Number in the code where this is called!
#define JOYSTR_CUR_LINE  (FString::FromInt(__LINE__))

//Current Function Signature where this is called!
#define JOYSTR_CUR_FUNCSIG (FString(__FUNCSIG__))

//Current Class and Line Number where this is called!
#define JOYSTR_CUR_CLASS_LINE (JOYSTR_CUR_CLASS_FUNC + "(" + JOYSTR_CUR_LINE + ")")


DECLARE_LOG_CATEGORY_EXTERN(NetLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(MgrTick, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(TestUI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(HuangYong, Log, All);


#define LLOG_NET(FormatString , ...)            UE_LOG(NetLog,      Log, TEXT("%s: %s"),   *JOYSTR_CUR_CLASS_LINE, *FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ))
#define LLOG_MGRTICK(FormatString , ...)        UE_LOG(MgrTick,     Log, TEXT("%s: %s"),   *JOYSTR_CUR_CLASS_LINE, *FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ))
#define LLOG_UI(FormatString , ...)             UE_LOG(TestUI,      Log, TEXT("%s: %s"),   *JOYSTR_CUR_CLASS_LINE, *FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ))
#define LLOG_HY(FormatString , ...)             UE_LOG(HuangYong,   Log, TEXT("%s: %s"),   *JOYSTR_CUR_CLASS_LINE, *FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ))




