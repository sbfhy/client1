#pragma once

#include "service/c2g_user.pb.h"
#include "System/Subsystem/create_dynamic.h"
#include "System/Subsystem/MgrMessage.h"

class UMgrMessage;

namespace RPC {

class C2G_UserService_Stub : public CreateDynamicService<C2G_UserService_Stub, UMgrMessage>
                           , public CMD::C2G_UserService_Stub
{
public:
    C2G_UserService_Stub();
    C2G_UserService_Stub(UMgrMessage*);

    virtual void C2G_UserSignIn_DoneCb(const ::CMD::C2G_UserSignInArgPtr& request,
                                       const ::CMD::C2G_UserSignInResPtr& response) override;
    virtual void C2G_UserSignIn_TimeOut(const ::CMD::C2G_UserSignInArgPtr& request) override;

};

}   // namespace RPC

