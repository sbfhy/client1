//#include "Managers.h"
//
//#include "ThirdPersonMP/Base/Define/DefineVariable.h"
//#include "ThirdPersonMP/Network/MgrNetwork.h"
//
//#define  REGISTER_MANAGER(NAME)                                                             \
//    do {                                                                                    \
//        std::type_index typeIdx = std::type_index(typeid(##NAME##));                        \
//        m_vecMgrIdx.push_back(typeIdx);                                                     \
//        m_mapIdx2pMgr[typeIdx] = NAME##::PInstance();                                       \
//        m_mapIdx2MgrName[typeIdx] = #NAME;                                                  \
//    } while(false)
//
//
//bool Managers::Init()
//{
//    auto funcInitManager = [this]()
//    {
//        m_vecMgrIdx.clear();
//        m_mapIdx2pMgr.clear();
//        m_mapIdx2MgrName.clear();
//        REGISTER_MANAGER(MgrNetwork);
//    };
//    funcInitManager();
//
//    auto funcCallManager = [](MgrBase* manager)
//    {
//        if (manager)
//        {
//            manager->Init();
//        }
//    };
//    foreachManagers(funcCallManager);
//
//    return true;
//}
//
//void Managers::Tick(UDWORD deltaMSec)
//{
//    auto funcCallManager = [deltaMSec](MgrBase* manager)
//    {
//        if (manager)
//        {
//            manager->Tick(deltaMSec);
//        }
//    };
//    foreachManagers(funcCallManager);
//}
//
//void Managers::End()
//{
//    m_mapIdx2pMgr.clear();
//    MgrNetwork::Destroy();
//}
//
