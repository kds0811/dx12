#pragma once
#include "GraphicCommonHeaders.h"
#include <string>

class RootSignature;
class InputLayout;

class Pso
{
protected:
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso = nullptr;
    const RootSignature* pRootSignature = nullptr;

public:
    Pso();
    void SetRootSignature(const RootSignature* pRootSig);
    const RootSignature* GetRootSignature() const;
    [[nodiscard]] ID3D12PipelineState* GetPso() noexcept;
    [[nodiscard]] const ID3D12PipelineState* GetPso() const noexcept;
};

class GraphicPso : public Pso
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC mPsoDesc;

public:
    GraphicPso(const RootSignature* rootSignature);


    GraphicPso(const GraphicPso& rhs);
    GraphicPso(const GraphicPso&& rhs);
    GraphicPso& operator=(const GraphicPso& rhs);
    GraphicPso& operator=(const GraphicPso&& rhs);

    void SetRenderTargetBlendDesc(D3D12_RENDER_TARGET_BLEND_DESC& renderTargetBD, UINT slot = 0);
    void SetBlendState(const D3D12_BLEND_DESC& blendDesc);
    void SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc);
    void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc);
    void SetSampleMask(UINT sampleMask);
    void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType);
    void SetDepthTargetFormat(DXGI_FORMAT dsvFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
    void SetRenderTargetFormat(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
    void SetRenderTargetFormats(UINT numRTVs, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
    void SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE ibProps);
    void SetFillMode(D3D12_FILL_MODE fillMode);
    void SetCullMode(D3D12_CULL_MODE mode);
    void SetNumberRenderTargets(UINT num);
    void SetFrontCounterClockwise(bool state);
    void SetDepthFunc(D3D12_COMPARISON_FUNC func);
    void SetDepthEnable(bool state);
    void SetDepthWriteMask(D3D12_DEPTH_WRITE_MASK mask);

    inline void SetVertexShader(const D3D12_SHADER_BYTECODE& binary) { mPsoDesc.VS = binary; }
    inline void SetPixelShader(const D3D12_SHADER_BYTECODE& binary) { mPsoDesc.PS = binary; }
    inline void SetGeometryShader(const D3D12_SHADER_BYTECODE& binary) { mPsoDesc.GS = binary; }
    inline void SetHullShader(const D3D12_SHADER_BYTECODE& binary) { mPsoDesc.HS = binary; }
    inline void SetDomainShader(const D3D12_SHADER_BYTECODE& binary) { mPsoDesc.DS = binary; }
    void Finalize(std::wstring name, const InputLayout* inputLayouts);
};

class ComputePso : public Pso
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC mPsoDesc;

public:
    ComputePso();
    void SetComputeShader(const void* Binary, size_t Size) { mPsoDesc.CS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
    void SetComputeShader(const D3D12_SHADER_BYTECODE& Binary) { mPsoDesc.CS = Binary; }
    void Finalize(std::wstring name);
};
