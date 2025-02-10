#pragma once
#include <assimp/Importer.hpp>   // C++ importer interface
#include <assimp/scene.h>        // Output data structure
#include <assimp/postprocess.h>  // Post processing flags
#include "GeometryGenerator.h"

class AssimpLoader
{
public:
    GeometryGenerator::MeshData LoadGeometryFromFile(std::string filePath);

};