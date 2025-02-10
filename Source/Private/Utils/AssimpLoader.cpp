#include "AssimpLoader.h"
#include <cassert>
#include <stdexcept>

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
        for (size_t j = 0; j < scene->mMeshes[i]->mNumVertices; ++j)
        {
            GeometryGenerator::Vertex v;

            v.Position =
                DirectX::XMFLOAT3{scene->mMeshes[i]->mVertices[j].x, scene->mMeshes[i]->mVertices[j].y, scene->mMeshes[i]->mVertices[j].z};

            v.Normal =
                DirectX::XMFLOAT3{scene->mMeshes[i]->mNormals[j].x, scene->mMeshes[i]->mNormals[j].y, scene->mMeshes[i]->mNormals[j].z};

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
