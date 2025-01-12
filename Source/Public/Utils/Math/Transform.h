#pragma once
#include <DirectXMath.h>
#include "Vector.h"
#include "Rotator.h"

struct alignas(16) Transform final
{
private:
    DirectX::XMFLOAT4A Translation;
    DirectX::XMFLOAT4A Rotation; // as a quaternion
    DirectX::XMFLOAT4A Scale3D;

public:
    inline Transform() noexcept : Translation(0.0f, 0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f, 1.0f), Scale3D(1.0f, 1.0f, 1.0f, 0.0f) {}


};