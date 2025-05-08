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
    Pso(ID3D12PipelineState* pso, const RootSignature* const rootSignature);
    void SetRootSignature(const RootSignature* pRootSig);
    const RootSignature* GetRootSignature() const;
    [[nodiscard]] ID3D12PipelineState* GetPso() noexcept;
    [[nodiscard]] const ID3D12PipelineState* GetPso() const noexcept;
};



class GraphicsPso : public Pso
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC mPsoDesc;
    const InputLayout* pInputLayouts = nullptr;

public:
    GraphicsPso(const RootSignature* rootSignature, const InputLayout* inputLayouts);
    GraphicsPso(const GraphicsPso& rhs);
    GraphicsPso(const GraphicsPso&& rhs);
    GraphicsPso& operator=(const GraphicsPso& rhs);
    GraphicsPso& operator=(const GraphicsPso&& rhs);

    void SetBlendState(const D3D12_BLEND_DESC& BlendDesc);
    void SetRasterizerState(const D3D12_RASTERIZER_DESC& RasterizerDesc);
    void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc);
    void SetSampleMask(UINT SampleMask);
    void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType);
    void SetDepthTargetFormat(DXGI_FORMAT DSVFormat, UINT MsaaCount = 1, UINT MsaaQuality = 0);
    void SetRenderTargetFormat(DXGI_FORMAT RTVFormat, DXGI_FORMAT DSVFormat, UINT MsaaCount = 1, UINT MsaaQuality = 0);
    void SetRenderTargetFormats(UINT NumRTVs, const DXGI_FORMAT* RTVFormats, DXGI_FORMAT DSVFormat, UINT MsaaCount = 1, UINT MsaaQuality = 0);
    void SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBProps);

    void SetVertexShader(const void* Binary, size_t Size) { mPsoDesc.VS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
    void SetPixelShader(const void* Binary, size_t Size) { mPsoDesc.PS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
    void SetGeometryShader(const void* Binary, size_t Size) { mPsoDesc.GS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
    void SetHullShader(const void* Binary, size_t Size) { mPsoDesc.HS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
    void SetDomainShader(const void* Binary, size_t Size) { mPsoDesc.DS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }

    void SetVertexShader(const D3D12_SHADER_BYTECODE& Binary) { mPsoDesc.VS = Binary; }
    void SetPixelShader(const D3D12_SHADER_BYTECODE& Binary) { mPsoDesc.PS = Binary; }
    void SetGeometryShader(const D3D12_SHADER_BYTECODE& Binary) { mPsoDesc.GS = Binary; }
    void SetHullShader(const D3D12_SHADER_BYTECODE& Binary) { mPsoDesc.HS = Binary; }
    void SetDomainShader(const D3D12_SHADER_BYTECODE& Binary) { mPsoDesc.DS = Binary; }
    void Finalize(std::wstring name);
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
