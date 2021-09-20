#pragma once

#include "service/c2g_scene.pb.h"
#include "System/Subsystem/create_dynamic.h"
#include "System/Subsystem/MgrMessage.h"

class UMgrMessage;

namespace RPC {

class C2G_SceneEntryService_Stub : public CreateDynamicService<C2G_SceneEntryService_Stub, UMgrMessage>
                                 , public CMD::C2G_SceneEntryService_Stub
{
public:
    C2G_SceneEntryService_Stub() {}
    C2G_SceneEntryService_Stub(UMgrMessage*) {}
  
    virtual void C2G_CreateEntry(const ::CMD::C2G_CreateEntryArgPtr& request,
                                 const ::CMD::C2G_CreateEntryResPtr& response) override;

private:
    UMgrMessage* m_MgrMessage{ nullptr };
};

}   // namespace RPC

