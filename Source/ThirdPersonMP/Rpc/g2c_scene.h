#pragma once

#include "service/g2c_scene.pb.h"
#include "System/Subsystem/create_dynamic.h"
#include "System/Subsystem/MgrMessage.h"

class UMgrMessage;

namespace RPC {

class G2C_SceneService : public CreateDynamicService<G2C_SceneService, UMgrMessage>
                       , public CMD::G2C_SceneService
{
public:
    G2C_SceneService();
    G2C_SceneService(UMgrMessage*);

    virtual void G2C_EnterScene(const ::CMD::G2C_EnterSceneArgPtr& request,
                                const ::CMD::EmptyResponsePtr& response,
                                void* args);

    virtual void G2C_CreatePlayerPawn(const ::CMD::G2C_CreatePlayerPawnArgPtr& request,
                                      const ::CMD::EmptyResponsePtr& response,
                                      void* args);

private:
    UMgrMessage* m_MgrMessage{ nullptr };
};

}   // namespace RPC
