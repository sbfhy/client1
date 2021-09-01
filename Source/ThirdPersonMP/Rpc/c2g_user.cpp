#include "c2g_user.h"

#include "Base/Log/Logger.h"

using namespace RPC;


C2G_UserLoginService_Stub::C2G_UserLoginService_Stub()
{

}

C2G_UserLoginService_Stub::C2G_UserLoginService_Stub(UMgrMessage*)
{

}

void C2G_UserLoginService_Stub::C2G_UserLogin_DoneCb(const ::CMD::C2G_UserLoginArgPtr& request,
                                                     const ::CMD::C2G_UserLoginResPtr& response)
{
    if (response)
    {
        LLOG_NET("%d", response->allow_login());
    }
}

void C2G_UserLoginService_Stub::C2G_UserLogin_TimeOut(const ::CMD::C2G_UserLoginArgPtr& request)
{

}

