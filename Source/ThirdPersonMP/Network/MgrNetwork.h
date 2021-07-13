//#pragma once
//
//#include "ThirdPersonMP/Base/Common/Singleton.h"
//#include "ThirdPersonMP/Base/Define/DefineVariable.h"
//#include "ThirdPersonMP/Base/Common/MgrBase.h"
//#include "ThirdPersonMP/Base/Define/DefineVariable.h"
//#include <memory>
//
//class MgrNetwork : public MgrBase, public ISingleton<MgrNetwork>
//{
//    friend class ISingleton<MgrNetwork>;
//private:
//    //MgrNetwork() = default;
//    //virtual ~MgrNetwork() = default;
//    MgrNetwork();
//    virtual ~MgrNetwork();
//
//public:
//    virtual bool Init() override;
//    virtual void Tick(UDWORD deltaMSec) override;    
//
//private:
//    std::shared_ptr<class TcpConnection>       m_connection;
//};
//
