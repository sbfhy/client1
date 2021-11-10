#include "g2c_scene.h"

#include "Base/Log/Logger.h"
#include "service/c2g_scene.pb.h"

#include "ThirdPersonMP/System/Subsystem/MgrActor.h"
#include "ThirdPersonMP/ThirdPersonMPGameMode.h"
#include "Kismet/GameplayStatics.h"

using namespace RPC;

G2C_SceneService::G2C_SceneService()
{

}

G2C_SceneService::G2C_SceneService(UMgrMessage* pMgrMessage)
{
    m_MgrMessage = pMgrMessage;
}

void G2C_SceneService::G2C_EnterScene(const ::CMD::G2C_EnterSceneArgPtr& request,
                                      const ::CMD::EmptyResponsePtr& response,
                                      void* args)
{
    if (!m_MgrMessage) return;
    LLOG_HY("%p", m_MgrMessage->GetWorld());
    UGameplayStatics::OpenLevel(m_MgrMessage->GetWorld(), TEXT("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap"), false);
}

void G2C_SceneService::G2C_CreatePlayerPawn(const ::CMD::G2C_CreatePlayerPawnArgPtr& request,
                                            const ::CMD::EmptyResponsePtr& response,
                                            void* args)
{
    LLOG_HY("G2C_SceneService::G2C_CreatePlayerPawn");
    
    UGameInstance* pGameInstance = m_MgrMessage->GetGameInstance();
    if (!pGameInstance) return;
    UMgrActor* pMgrActor = pGameInstance->GetSubsystem<UMgrActor>();
    if (!pMgrActor) return;
    AThirdPersonMPGameMode* pGameMode = Cast<AThirdPersonMPGameMode>(m_MgrMessage->GetWorld()->GetAuthGameMode());
    APawn* pPawn = pMgrActor->SpawnActor();
    if (!pPawn) return;
    pGameMode->ChangeApawn(pPawn);
}
