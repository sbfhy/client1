#pragma once

#include "Math/Vector.h"
#include "message/scene/pos.pb.h"
#include "Base/Define/DefineVariable.h"

#define  RotateScale 1000                   // 角度数值放大1000倍

inline CMD::Pos FVector2CMDPos(const FVector &vec)
{
    CMD::Pos ret;
    ret.set_x(vec.X);
    ret.set_y(vec.Y);
    ret.set_z(vec.Z);
    return ret;
}

inline DWORD FRotator2Rotate(const FRotator& rotator)
{
    return rotator.Yaw * RotateScale;
}
