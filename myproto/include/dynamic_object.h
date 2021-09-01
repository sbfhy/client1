#pragma once

#include <memory>

// 动态对象基类
class DynamicObject
{
public:
    DynamicObject() {}
    virtual ~DynamicObject() {}
};

typedef std::shared_ptr<DynamicObject> DynamicObjectPtr;
