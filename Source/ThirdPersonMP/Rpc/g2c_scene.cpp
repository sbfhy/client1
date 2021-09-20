#include "g2c_scene.h"

#include "Base/Log/Logger.h"
#include "service/c2g_scene.pb.h"

using namespace RPC;

G2C_SceneService::G2C_SceneService()
{

}

G2C_SceneService::G2C_SceneService(UMgrMessage* pMgrMessage)
{
    m_MgrMessage = pMgrMessage;
}

void G2C_SceneService::G2C_EnterScene(const ::CMD::G2C_EnterSceneArgPtr& request,
                                      const ::CMD::G2C_EnterSceneResPtr& response)
{
    LLOG_NET("G2C_SceneService::G2C_EnterScene");
    auto C2G_CreateEntryArgPtr = std::make_shared<CMD::C2G_CreateEntryArg>();
    if (m_MgrMessage)
    {
        m_MgrMessage->Send(C2G_CreateEntryArgPtr);
    }
}
