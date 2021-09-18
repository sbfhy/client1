#pragma once

#include "service/c2a_user.pb.h"
#include "System/Subsystem/create_dynamic.h"
#include "System/Subsystem/MgrMessage.h"

class UMgrMessage;

namespace RPC {

class C2A_UserService_Stub : public CreateDynamicService<C2A_UserService_Stub, UMgrMessage>
                           , public CMD::C2A_UserService_Stub
{
public:
    C2A_UserService_Stub();
    C2A_UserService_Stub(UMgrMessage*);

    virtual void C2A_UserSignIn_DoneCb(const ::CMD::C2A_UserSignInArgPtr& request,
                                       const ::CMD::C2A_UserSignInResPtr& response);
    virtual void C2A_UserSignIn_TimeOut(const ::CMD::C2A_UserSignInArgPtr& request);

private:
    UMgrMessage* m_MgrMessage{ nullptr };
};

}   // namespace RPC

