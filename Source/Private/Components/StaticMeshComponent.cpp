#include "StaticMeshComponent.h"
#include "Logger.h"

StaticMeshComponent::StaticMeshComponent(SceneObject* owner) : MeshComponent(owner)
{
   
}

void StaticMeshComponent::SetMesh(Mesh* mesh) 
{
    auto stMesh = dynamic_cast<StaticMesh*>(mesh);
    if (!stMesh)
    {
        LOG_WARNING("Dynamic cast is failed on Set Mesh Fenction. Object name: ", GetOwner()->GetName());
        return;
    }

    pStaticMesh = stMesh;
    LOG_MESSAGE("Object name: ", GetOwner()->GetName(), " changed static mesh");
}
