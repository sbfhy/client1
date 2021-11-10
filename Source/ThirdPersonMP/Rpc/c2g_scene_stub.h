#pragma once

#include "service/c2g_scene.pb.h"
#include "System/Subsystem/create_dynamic.h"
#include "System/Subsystem/MgrMessage.h"

class UMgrMessage;

namespace RPC {

class C2G_SceneService_Stub : public CreateDynamicService<C2G_SceneService_Stub, UMgrMessage>
                            , public CMD::C2G_SceneService_Stub
{
public:
    C2G_SceneService_Stub();
    C2G_SceneService_Stub(UMgrMessage*);
  
    virtual void C2G_NotifyLoadedScene(const ::CMD::C2G_NotifyLoadedSceneArgPtr& request,
                                       const ::CMD::EmptyResponsePtr& response,
                                       void* args) override;

private:
    UMgrMessage* m_MgrMessage{ nullptr };
};

}   // namespace RPC

