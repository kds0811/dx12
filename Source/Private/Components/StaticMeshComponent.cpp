#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent(SceneObject* owner) : MeshComponent(owner)
{
   
}

void StaticMeshComponent::SetRenderData(RenderData* rd) 
{
    mStaticMesh->SetRenderData(rd);
}
