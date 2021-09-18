#include "c2g_user.h"

#include "Base/Log/Logger.h"

using namespace RPC;


C2G_UserService_Stub::C2G_UserService_Stub()
{

}

C2G_UserService_Stub::C2G_UserService_Stub(UMgrMessage*)
{

}

void C2G_UserService_Stub::C2G_UserSignIn_DoneCb(const ::CMD::C2G_UserSignInArgPtr& request,
                                                 const ::CMD::C2G_UserSignInResPtr& response)
{
    if (response)
    {
        LLOG_NET("%d", response->allow_sign_in());
    }
}

void C2G_UserService_Stub::C2G_UserSignIn_TimeOut(const ::CMD::C2G_UserSignInArgPtr& request)
{

}

