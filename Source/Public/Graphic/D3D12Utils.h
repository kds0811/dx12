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
#include "Vector.h"
#include "GraphicGlobals.h"

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

enum class ERenderLayer : std::uint8_t
{
    Opaque,
    Transparent,
    AlphaTested,
    Count,
    Mirror,
    Reflected,
    Shadow,
    GenerateTrees,
    GeometrySubdivide,
    TesselationInteger,
    TesselationFracOdd,
    TesselationFracEven,
    TesselationPow2,
    Skybox
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