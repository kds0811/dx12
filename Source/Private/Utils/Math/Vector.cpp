#include "Vector.h"

Vector::Vector()   
{
    Data.x = 0.0f;
    Data.y = 0.0f;
    Data.z = 0.0f;
}

Vector::Vector(float x, float y, float z) {}

Vector::Vector(DirectX::XMFLOAT3 Vector) {}
