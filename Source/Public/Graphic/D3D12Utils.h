#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <directx/d3dx12.h>
#include "GraphicError.h"
#include <unordered_map>
#include <DirectXMath.h>
#include "MathHelper.h"
#include "SceneComponent.h"
#include <cstdint>
#include <array>

class D3D12Utils
{
    
public:
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
        const void* initData, UINT64 byteSize, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

    static UINT CalcConstantBufferByteSize(UINT byteSize) { return (byteSize + 255) & ~255; }

    static Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::string& filename);

    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);
};

enum class EPrimitiveType : std::uint8_t
{
    BOX,
    GRID,
    CYLINDER,
    SPHERE,
    LAND,
    MESH,
    WAVES,
    MODEL,
    MODEL1,
    MODELSPIDERFBX,
    MODELSPIDEROBJ,
    HUESITOS
};

struct SubmeshGeometry
{
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    INT BaseVertexLocation = 0;
};

struct MeshGeometry
{
    // Give it a name so we can look it up by name.
    std::string Name;

    // System memory copies.  Use Blobs because the vertex/index format can be generic.
    // It is up to the client to cast appropriately.
    Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

    // Data about the buffers.
    UINT VertexByteStride = 0;
    UINT VertexBufferByteSize = 0;
    DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
    UINT IndexBufferByteSize = 0;

    // A MeshGeometry may store multiple geometries in one vertex/index buffer.
    // Use this container to define the Submesh geometries so we can draw
    // the Submeshes individually.
    std::unordered_map<EPrimitiveType, SubmeshGeometry> DrawArgs;

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
    {
        D3D12_VERTEX_BUFFER_VIEW vbv{};
        vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
        vbv.StrideInBytes = VertexByteStride;
        vbv.SizeInBytes = VertexBufferByteSize;

        return vbv;
    }


    D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
    {
        D3D12_INDEX_BUFFER_VIEW ibv{};
        ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
        ibv.Format = IndexFormat;
        ibv.SizeInBytes = IndexBufferByteSize;

        return ibv;
    }

    // We can free this memory after we finish upload to the GPU.
    void DisposeUploaders()
    {
        VertexBufferUploader = nullptr;
        IndexBufferUploader = nullptr;
    }
};


struct Light
{
    DirectX::XMFLOAT3 Strength = {0.5f, 0.5f, 0.5f};
    float FalloffStart = 1.0f;                          // point/spot light only
    DirectX::XMFLOAT3 Direction = {0.0f, -1.0f, 0.0f};  // directional/spot light only
    float FalloffEnd = 10.0f;                           // point/spot light only
    DirectX::XMFLOAT3 Position = {0.0f, 0.0f, 0.0f};    // point/spot light only
    float SpotPower = 64.0f;                            // spot light only
};

#define MaxLights 16

struct MaterialConstants
{
    DirectX::XMFLOAT4 DiffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
    DirectX::XMFLOAT3 FresnelR0 = {0.01f, 0.01f, 0.01f};
    float Roughness = 0.25f;

    // Used in texture mapping.
    DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

enum class EMaterialType : std::uint8_t
{
    GRASS,
    GRASSX,
    GRASSY,
    WATER,
    BRICKS,
    STONE,
    TILE,
    SKULLMAT,
    UNKNOWN,
    WOODCRATE,
    FIREBALL,
    COLDFIRE

};

struct Texture;

struct Material
{
    // Unique material name for lookup.
    EMaterialType Type;

    // Index into constant buffer corresponding to this material.
    int MatCBIndex = -1;

    Texture* Tex = nullptr;

    // Dirty flag indicating the material has changed and we need to update the constant buffer.
    // Because we have a material constant buffer for each FrameResource, we have to apply the
    // update to each FrameResource.  Thus, when we modify a material we should set
    // NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
    int NumFramesDirty = gNumFrameResources;

    // Material constant buffer data used for shading.
    DirectX::XMFLOAT4 DiffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
    DirectX::XMFLOAT3 FresnelR0 = {0.01f, 0.01f, 0.01f};
    float Roughness = .25f;

    DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};


struct Texture
{
    // Unique material name for lookup.
    EMaterialType Type;

    std::wstring Filename;

    // Index into SRV heap for diffuse texture.
    int DiffuseSrvHeapIndex = -1;

    // Index into SRV heap for normal texture.
    int NormalSrvHeapIndex = -1;

    Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};


static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers()
{
    // Applications usually only need a handful of samplers.  So just define them all up front
    // and keep them available as part of the root signature.

    const CD3DX12_STATIC_SAMPLER_DESC pointWrap(0,  // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT,             // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,            // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,            // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP);           // addressW

    const CD3DX12_STATIC_SAMPLER_DESC pointClamp(1,  // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT,              // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,            // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,            // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP);           // addressW

    const CD3DX12_STATIC_SAMPLER_DESC linearWrap(2,  // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,             // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,             // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,             // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP);            // addressW

    const CD3DX12_STATIC_SAMPLER_DESC linearClamp(3,  // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,              // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,             // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,             // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP);            // addressW

    const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(4,  // shaderRegister
        D3D12_FILTER_ANISOTROPIC,                         // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,                  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,                  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,                  // addressW
        0.0f,                                             // mipLODBias
        8);                                               // maxAnisotropy

    const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(5,  // shaderRegister
        D3D12_FILTER_ANISOTROPIC,                          // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,                  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,                  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,                  // addressW
        0.0f,                                              // mipLODBias
        8);                                                // maxAnisotropy

    return {pointWrap, pointClamp, linearWrap, linearClamp, anisotropicWrap, anisotropicClamp};
}