#include "c2a_user_stub.h"

#include "Base/Log/Logger.h"
#include "ThirdPersonMP/ThirdPersonMPGameMode.h"
#include "ThirdPersonMP/ThirdPersonMPCharacter.h"


using namespace RPC;


C2A_UserService_Stub::C2A_UserService_Stub()
{

}

C2A_UserService_Stub::C2A_UserService_Stub(UMgrMessage* pMgrMessage)
    : m_MgrMessage(pMgrMessage)
{

}

void C2A_UserService_Stub::C2A_UserSignIn_DoneCb(const ::CMD::C2A_UserSignInArgPtr& request,
                                                 const ::CMD::C2A_UserSignInResPtr& response)
{
    if (response && response->allow_sign_in())
    {
        LLOG_NET("accid:%d", request->accid());
        if (m_MgrMessage && m_MgrMessage->GetWorld())
        {
            m_MgrMessage->SetAccid(request->accid());
        }
    }
}

void C2A_UserService_Stub::C2A_UserSignIn_TimeOut(const ::CMD::C2A_UserSignInArgPtr& request)
{

}

