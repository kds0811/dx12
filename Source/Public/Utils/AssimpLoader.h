#pragma once
#include "GeometryGenerator.h"
#include <string>

class AssimpLoader
{
public:
    GeometryGenerator::MeshData LoadGeometryFromFile(std::string filePath);

};