#pragma once
#include <DirectXMath.h>

class Vector3
{
public:
    Vector3();
    Vector3(float x, float y, float z);
    explicit Vector3(DirectX::XMFLOAT3 Vector);


private:
    DirectX::XMFLOAT3A Data;
};