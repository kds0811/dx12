#include "GeometryLoader.h"
#include <fstream>
#include <Windows.h>

GeometryGenerator::MeshData GeometryLoader::LoadGeometryFromFile(std::string filePath)
{
    std::ifstream fin(filePath);
    if (!fin)
    {
        throw std::runtime_error("Cannot open file: " + filePath);
    }

    MeshData meshData;
    UINT vcount = 0;
    UINT tcount = 0;
    std::string ignore;

    if (!(fin >> ignore >> vcount)) throw std::runtime_error("Error reading vertex count");

    if (!(fin >> ignore >> tcount)) throw std::runtime_error("Error reading triangle count");

    fin >> ignore >> ignore >> ignore >> ignore;

    meshData.Vertices.resize(vcount);
    for (UINT i = 0; i < vcount; ++i)
    {
        if (!(fin >> meshData.Vertices[i].Position.x >> meshData.Vertices[i].Position.y >> meshData.Vertices[i].Position.z))
            throw std::runtime_error("Error reading vertex position");

        if (!(fin >> meshData.Vertices[i].Normal.x >> meshData.Vertices[i].Normal.y >> meshData.Vertices[i].Normal.z))
            throw std::runtime_error("Error reading vertex normal");
    }

    fin >> ignore >> ignore >> ignore;

    meshData.Indices32.resize(3 * tcount);
    for (UINT i = 0; i < tcount; ++i)
    {
        if (!(fin >> meshData.Indices32[i * 3 + 0] >> meshData.Indices32[i * 3 + 1] >> meshData.Indices32[i * 3 + 2]))
            throw std::runtime_error("Error reading triangle indices");
    }

    fin.close();
    return meshData;
}