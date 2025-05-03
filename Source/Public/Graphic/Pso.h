#pragma once
#include "GraphicCommonHeaders.h"
#include <string>

class RootSignature;

class Pso
{
protected:
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso = nullptr;
    std::wstring mName{};
    const RootSignature* pRootSignature = nullptr;

public:
    Pso(const std::wstring& name);
    void SetRootSignature(const RootSignature* pRootSig);
    const RootSignature* GetRootSignature() const;
    [[nodiscard]] ID3D12PipelineState* GetPso() noexcept;
    [[nodiscard]] const ID3D12PipelineState* GetPso() const noexcept;
};



class GraphicsPso : public Pso
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC mPsoDesc;
    std::unique_ptr<const D3D12_INPUT_ELEMENT_DESC> mInputLayouts;

public:
    GraphicsPso(const std::wstring& Name = L"Unnamed Graphics PSO");

    void SetBlendState(const D3D12_BLEND_DESC& BlendDesc);
    void SetRasterizerState(const D3D12_RASTERIZER_DESC& RasterizerDesc);
    void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc);
    void SetSampleMask(UINT SampleMask);
    void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType);
    void SetDepthTargetFormat(DXGI_FORMAT DSVFormat, UINT MsaaCount = 1, UINT MsaaQuality = 0);
    void SetRenderTargetFormat(DXGI_FORMAT RTVFormat, DXGI_FORMAT DSVFormat, UINT MsaaCount = 1, UINT MsaaQuality = 0);
    void SetRenderTargetFormats(UINT NumRTVs, const DXGI_FORMAT* RTVFormats, DXGI_FORMAT DSVFormat, UINT MsaaCount = 1, UINT MsaaQuality = 0);
    void SetInputLayout(UINT NumElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs);
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
    void Finalize();
};

class ComputePso : public Pso
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC mPsoDesc;

public:
    ComputePso(const std::wstring& Name = L"Unnamed Compute PSO");
    void SetComputeShader(const void* Binary, size_t Size) { mPsoDesc.CS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
    void SetComputeShader(const D3D12_SHADER_BYTECODE& Binary) { mPsoDesc.CS = Binary; }
    void Finalize();
};
