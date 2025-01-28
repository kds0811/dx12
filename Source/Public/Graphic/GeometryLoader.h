#pragma once
#include "GeometryGenerator.h"
#include <string>


class GeometryLoader : public GeometryGenerator
{
public:
    GeometryLoader() = default;

    GeometryGenerator::MeshData LoadGeometryFromTXTFile(std::string filePath);
};
