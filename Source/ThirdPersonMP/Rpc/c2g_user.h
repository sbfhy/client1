#pragma once

#include "service/c2g_user.pb.h"
#include "System/Subsystem/create_dynamic.h"
#include "System/Subsystem/MgrMessage.h"

class UMgrMessage;

namespace RPC {

class C2G_UserLoginService_Stub : public CreateDynamicService<C2G_UserLoginService_Stub, UMgrMessage>
                                , public CMD::C2G_UserLoginService_Stub
{
public:
    C2G_UserLoginService_Stub();
    C2G_UserLoginService_Stub(UMgrMessage*);

    virtual void C2G_UserLogin_DoneCb(const ::CMD::C2G_UserLoginArgPtr& request,
                                      const ::CMD::C2G_UserLoginResPtr& response) override;
    virtual void C2G_UserLogin_TimeOut(const ::CMD::C2G_UserLoginArgPtr& request) override;

};

}   // namespace RPC

