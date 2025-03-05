#include "BoundComponent.h"

void BoundComponent::Init(Vector min, Vector max)
{
    mCenter = (min + max) * 0.5f;
    mExtents = (max - min) * 0.5f;
    DirectX::XMStoreFloat3(&mAABB.Center, mCenter.ToSIMD());
    DirectX::XMStoreFloat3(&mAABB.Extents, mExtents.ToSIMD());
}


void BoundComponent::SetVisible(bool isVisible) 
{
    bIsVisible = isVisible;
    
}
