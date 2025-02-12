#pragma once

#include "D3D12Utils.h"
#include "MathHelper.h"
#include "UploadBuffer.h"

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
};

struct PassConstants
{
    DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
    DirectX::XMFLOAT3 EyePosW = {0.0f, 0.0f, 0.0f};
    float cbPerObjectPad1 = 0.0f;
    DirectX::XMFLOAT2 RenderTargetSize = {0.0f, 0.0f};
    DirectX::XMFLOAT2 InvRenderTargetSize = {0.0f, 0.0f};
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;

    DirectX::XMFLOAT4 AmbientLight = {0.25f, 0.25f, 0.35f, 1.0f};

    DirectX::XMFLOAT4 FogColor = {0.7f, 0.7f, 0.7f, 1.0f};
    float gFogStart = 40.0f;
    float gFogRange = 300.0f;
    DirectX::XMFLOAT2 cbPerObjectPad2;

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light Lights[MaxLights];

};

struct Vertex
{
    DirectX::XMFLOAT3 Pos{0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 Normal = {0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT2 TexC = {0.0f, 0.0f};
};



// Stores the resources needed for the CPU to build the command lists
// for a frame.
struct FrameResource
{
public:
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;
    std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
    std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
    std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
    std::unique_ptr<UploadBuffer<Vertex>> WavesVB = nullptr;

    UINT64 Fence = 0;

public:
    FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT waveVertCount, UINT materialCount);
    FrameResource(const FrameResource& rhs) = delete;
    FrameResource& operator=(const FrameResource& rhs) = delete;
    FrameResource(const FrameResource&& rhs) = delete;
    FrameResource& operator=(const FrameResource&& rhs) = delete;
    ~FrameResource();

};