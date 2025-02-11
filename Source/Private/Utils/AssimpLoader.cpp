#include "AssimpLoader.h"
#include <cassert>
#include <stdexcept>
#include <assimp/Importer.hpp>   // C++ importer interface
#include <assimp/scene.h>        // Output data structure
#include <assimp/postprocess.h>  // Post processing flags


GeometryGenerator::MeshData AssimpLoader::LoadGeometryFromFile(std::string filePath)
{
    Assimp::Importer importer;
    GeometryGenerator::MeshData result{};
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                                           aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error(importer.GetErrorString());
    }

    for (size_t i = 0; i < scene->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        for (size_t j = 0; j < mesh->mNumVertices; ++j)
        {
            GeometryGenerator::Vertex v;
            v.Position = DirectX::XMFLOAT3{mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z};
            v.Normal = DirectX::XMFLOAT3{mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z};

            if (mesh->HasTextureCoords(0)) 
            {
                v.TexC = DirectX::XMFLOAT2{mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y};
            }
            else
            {
                v.TexC = DirectX::XMFLOAT2{0.0f, 0.0f};
            }

            result.Vertices.push_back(v);
        }
    }

    size_t vertexOffset = 0;
    for (size_t i = 0; i < scene->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[i];

        for (size_t j = 0; j < mesh->mNumFaces; ++j)
        {
            const aiFace& face = mesh->mFaces[j];

            if (face.mNumIndices != 3)
            {
                throw std::runtime_error("Non-triangular face detected!");
            }
            result.Indices32.push_back(face.mIndices[0] + vertexOffset);
            result.Indices32.push_back(face.mIndices[1] + vertexOffset);
            result.Indices32.push_back(face.mIndices[2] + vertexOffset);
        }
        vertexOffset += mesh->mNumVertices;
    }

    return result;
}
