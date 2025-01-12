#pragma once
#include <DirectXMath.h>

class alignas(16) Transform
{
private:
    DirectX::XMFLOAT4A Translation;
    DirectX::XMFLOAT4A Rotation;
    DirectX::XMFLOAT4A Scale3D;

public:


};