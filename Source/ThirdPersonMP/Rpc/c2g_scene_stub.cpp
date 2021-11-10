#include "c2g_scene_stub.h"

#include "Base/Log/Logger.h"

using namespace RPC;

C2G_SceneService_Stub::C2G_SceneService_Stub()
{

}

C2G_SceneService_Stub::C2G_SceneService_Stub(UMgrMessage* pMgrMessage)
    : m_MgrMessage(pMgrMessage)
{

}

void C2G_SceneService_Stub::C2G_NotifyLoadedScene(const ::CMD::C2G_NotifyLoadedSceneArgPtr& request,
                                                  const ::CMD::EmptyResponsePtr& response,
                                                  void* args) 
{
    LLOG_NET("");
}
