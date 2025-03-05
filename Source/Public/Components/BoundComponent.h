#pragma once
#include "Vector.h"
#include "DirectXMath.h"
#include "DirectXCollision.h"

class BoundComponent
{
    Vector mCenter = Vector::One();
    Vector mExtents = Vector::One();
    bool bIsVisible = false;
    DirectX::BoundingBox mAABB;

public:
    BoundComponent() = default;
    void Init(Vector min, Vector max);

    void SetVisible(bool isVisible);

};