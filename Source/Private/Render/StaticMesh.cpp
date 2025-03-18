#include "StaticMesh.h"

StaticMesh::StaticMesh(std::string name, int ID) : Mesh(name, ID) {}

void StaticMesh::SetRenderData(RenderData* rd) 
{
    mRenderData = rd;
}
