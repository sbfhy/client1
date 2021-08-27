#include "c2g_userloginservice.h"
#include "Base/Log/Logger.h"

void RPC::C2G_UserLoginService_Stub::C2G_UserLogin_Solved(CMD::C2G_UserLoginRes* response)
{
    LLOG_NET("allow_login:%s", response->allow_login() ? *FString("True") : *FString("False"));
}
