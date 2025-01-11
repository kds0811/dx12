#pragma once
#include <DirectXMath.h>

class Vector
{
public:
    Vector();
    Vector(float x, float y, float z);
    explicit Vector(DirectX::XMFLOAT3 Vector);


private:
    DirectX::XMFLOAT3A Data;
};