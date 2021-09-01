#pragma once

#include "dynamic_object.h"
#include "MgrDynamicfactory.h"

// 动态对象创建器
// 注意：使用gcc，CreateDynamic的派生类一定要显式声明构造函数，否则不会执行注册代码
template <typename T>
class CreateDynamic : public DynamicObject
{
public:
    static DynamicObject *CreateObject()
    {
        return new T();
    }

    struct Registor
    {
        Registor()
        {
            if (!MgrDynamicFactory::Instance().Regist(typeid(T).name(), CreateObject))
            {
                assert(false);
            }
        }

        inline void do_nothing() const {}
    };

    static Registor s_registor;

public:
    CreateDynamic()
    {
        s_registor.do_nothing();
    }

    virtual ~CreateDynamic()
    {
        s_registor.do_nothing();
    }
};

template <typename T>
typename CreateDynamic<T>::Registor CreateDynamic<T>::s_registor;


// ===================================================================

template <typename T, typename Mgr>
class CreateDynamicService
{
public:
    static DynamicObject *CreateObject(Mgr* mgr)
    {
        return new T(mgr);
    }

    struct Registor
    {
        Registor()
        {
            if (!MgrDynamicFactory::Instance().RegistService(typeid(T).name(), CreateObject))
            {
                assert(false);
            }
        }

        inline void do_nothing() const {}
    };

    static Registor s_registor;

public:
    CreateDynamicService()
    {
        s_registor.do_nothing();
    }

    virtual ~CreateDynamicService()
    {
        s_registor.do_nothing();
    }
};

template <typename T, typename Mgr>
typename CreateDynamicService<T, Mgr>::Registor CreateDynamicService<T, Mgr>::s_registor;

