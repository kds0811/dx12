#include "StaticMesh.h"

StaticMesh::StaticMesh(std::string name, int ID) : Mesh(name, ID) {}

void StaticMesh::SetGeoRenderData(GeoRenderData* rd)
{
    mGeoRenderData = rd;
}
