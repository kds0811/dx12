#pragma once
#include "StaticMesh.h"
#include "GeometryGenerator.h"

class StaticMeshFactory
{
    static int mMeshesCurrentID;
    GeometryGenerator mGeometryGenerator;

public:
    StaticMeshFactory();

    
};
