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

};

}   // namespace RPC
