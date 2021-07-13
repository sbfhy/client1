//#pragma once
//
//#include <functional>
//#include <unordered_map>
//#include <typeinfo>
//#include <typeindex>
//
//#include "ThirdPersonMP/Base/Common/Singleton.h"
//#include "ThirdPersonMP/Base/Common/MgrBase.h"
//#include "ThirdPersonMP/Base/Define/DefineVariable.h"
//
//class Managers  final
//{
//public:
//    Managers() = default;
//    virtual ~Managers() = default;
//
//    bool Init();
//    void Tick(UDWORD deltaMSec);
//    void End();
//
//private:
//    template<class T> void foreachManagers(T func)
//    {
//        for (auto index : m_vecMgrIdx)
//        {
//            auto itFind = m_mapIdx2pMgr.find(index);
//            if (itFind != m_mapIdx2pMgr.end() && itFind->second)
//            {
//                func(itFind->second);
//            }
//        }
//    }
//
//private:
//    std::vector<std::type_index> m_vecMgrIdx;
//    std::unordered_map<std::type_index, MgrBase*> m_mapIdx2pMgr;
//    std::unordered_map<std::type_index, std::string> m_mapIdx2MgrName;
//    std::unordered_map<std::type_index, std::function<void()>> m_mapIdx2MgrDelFunc;
//};
//
