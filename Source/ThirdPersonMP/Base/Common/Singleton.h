#pragma once

#include "ThirdPersonMP/Base/Common/Noncopyable.h"
#include "ThirdPersonMP/Base/Define/DefineNew.h"
#include "Logging/LogMacros.h"

template <typename T>
class ISingleton : public Noncopyable
{
protected:
    //ISingleton() = default;
    //virtual ~ISingleton() = default;
    ISingleton()
    {
        UE_LOG(LogTemp, Error, TEXT("ISingleton()"));
    }
    virtual ~ISingleton()
    {
        UE_LOG(LogTemp, Error, TEXT("~ISingleton()"));
    }

public:
    static T* PInstance()
    {
        if (m_pInstance == nullptr)
        {
            static CGarbo m_garbo;
            m_pInstance = NEW T();
            UE_LOG(LogTemp, Error, TEXT("ISingleton new T, %0x"), m_pInstance);
        }
        return m_pInstance;
    }

    static void Destroy()
    {
        if (m_pInstance)
        {
            UE_LOG(LogTemp, Error, TEXT("ISingleton Destroy() T, %0x"), m_pInstance);
            DELETE_SAFE(m_pInstance);
            m_pInstance = nullptr;
        }
    }

    static bool IsValid()
    {
        return m_pInstance != nullptr;
    }

private:
    class CGarbo
    {
    public:
        ~CGarbo()
        {
            DELETE_SAFE(m_pInstance);
        }
    };

private:
    static T* m_pInstance;
};

template <typename T> T* ISingleton<T>::m_pInstance = nullptr;

