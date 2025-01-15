#pragma once
#include <DirectXMath.h>
#include "Transform.h"

class alignas(16) Camera
{
    Transform Transformation;

public:
    inline Camera() noexcept : Transformation(Vector{0.f, 0.f, -10.f}, Rotator::Zero(), Vector(1.f, 1.f, 1.f)) {}

    void MoveRight(float dt);
    void MoveForward(float dt);



};