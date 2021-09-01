// 参考 [C++实现反射(根据类名动态创建对象](https://blog.csdn.net/heyuhang112/article/details/51729435)  

#pragma once

#ifdef __GNUC__
#include <cxxabi.h>
#endif // __GNUC__

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <typeinfo>
#include "Base/Common/Singleton.h"

#include "CoreMinimal.h"

class DynamicObject;
class UMgrMessage;

/**
 * 动态对象创建工厂
 */
class MgrDynamicFactory : public ISingleton<MgrDynamicFactory>
{
public:
    template <typename T>
    struct Registor             // 内部注册结构
    {
        Registor()
        {
            if (!MgrDynamicFactory::Instance().Regist(typeid(T).name(), CreateObject))
            {
                assert(false);
            }
        }
        static DynamicObject* CreateObject()
        {
            return new T();
        }
    };

public:
    typedef DynamicObject* (*CreateFunction)();

    // 解析类型名称（转换为 A::B::C 的形式）
    // GCC 的type_info::name()输出的名称很猥琐，这里只做简单的解析，只支持自定义的结构体（非模板），类（非模板）、枚举、联合
    static std::string ReadTypeName(const char* name);

    bool Regist(const char* name, CreateFunction func);
    DynamicObject* Create(const std::string& type_name);

    template <typename T>
    T* Create(const std::string& type_name);

    // Service
    typedef DynamicObject* (*CreateServiceFunc)(UMgrMessage*);
    bool RegistService(const char* name, CreateServiceFunc func);
    DynamicObject* CreateService(const std::string& type_name, UMgrMessage*);

private:
    std::map<std::string, CreateFunction> _create_function_map;
    std::map<std::string, CreateServiceFunc> m_mapCreateService;
};

#define RegistorDynamicObject(className)    static MgrDynamicFactory::Registor<className> 

