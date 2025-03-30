#pragma once
#include "GraphicCommonHeaders.h"
#include "MathHelper.h"

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 WorldInvTranspose = MathHelper::Identity4x4();
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

    DirectX::XMFLOAT4 AmbientLight = {0.15f, 0.15f, 0.25f, 1.0f};

    DirectX::XMFLOAT4 FogColor = {0.7f, 0.7f, 0.7f, 1.0f};
    float gFogStart = 200.0f;
    float gFogRange = 1000.0f;
    DirectX::XMFLOAT2 cbPerObjectPad2;

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light Lights[MaxLights];

    DirectX::XMFLOAT4 gBaseColors[32] = {DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),

        DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.25f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(0.75f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.25f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.25f, 1.0f),

        DirectX::XMFLOAT4(0.9f, 0.6f, 0.6f, 1.0f), DirectX::XMFLOAT4(0.6f, 0.9f, 0.6f, 1.0f), DirectX::XMFLOAT4(0.6f, 0.6f, 0.9f, 1.0f), DirectX::XMFLOAT4(0.9f, 0.9f, 0.6f, 1.0f),
        DirectX::XMFLOAT4(0.9f, 0.6f, 0.9f, 1.0f), DirectX::XMFLOAT4(0.6f, 0.9f, 0.9f, 1.0f),

        DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f), DirectX::XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f),
        DirectX::XMFLOAT4(0.5f, 0.25f, 0.0f, 1.0f)};
};

struct Vertex
{
    DirectX::XMFLOAT3 Pos{0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 Normal = {0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT2 TexC = {0.0f, 0.0f};
};
