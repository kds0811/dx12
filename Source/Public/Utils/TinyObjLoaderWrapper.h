#pragma once
#include "GeometryGenerator.h"
#include <string>

class TinyObjLoaderWrapper
{
public:
    GeometryGenerator::MeshData LoadGeometryFromFile(std::string filePath);
};