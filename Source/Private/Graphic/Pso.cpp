#include "Pso.h"
#include "RootSignature.h"
#include "Device.h"
#include "InputLayout.h"

Pso::Pso() = default;

void Pso::SetRootSignature(const RootSignature* pRootSig)
{
    assert(pRootSig);
    if (!pRootSig) LOG_ERROR("root signature argument is nullptr!");
    pRootSignature = pRootSig;
}

const RootSignature* Pso::GetRootSignature() const
{
    assert(pRootSignature);
    if (!pRootSignature) LOG_ERROR("pRootSignature is nullptr");
    return pRootSignature;
}

ID3D12PipelineState* Pso::GetPso() noexcept
{
    assert(mPso);
    if (!mPso) LOG_ERROR("mPso is nullptr");
    return mPso.Get();
}

const ID3D12PipelineState* Pso::GetPso() const noexcept
{
    assert(mPso);
    if (!mPso) LOG_ERROR("mPso is nullptr");
    return mPso.Get();
}

GraphicPso::GraphicPso(const RootSignature* rootSignature)
{
    SetRootSignature(rootSignature);
    ZeroMemory(&mPsoDesc, sizeof(mPsoDesc));
    mPsoDesc.NodeMask = 1;
    mPsoDesc.SampleMask = 0xFFFFFFFFu;
    mPsoDesc.SampleDesc.Count = 1;
    mPsoDesc.InputLayout.NumElements = 0;
}

GraphicPso::GraphicPso(const GraphicPso& rhs) : mPsoDesc(rhs.mPsoDesc)
{
    pRootSignature = rhs.pRootSignature;
}

GraphicPso::GraphicPso(const GraphicPso&& rhs) :  mPsoDesc(std::move(rhs.mPsoDesc))
{
    pRootSignature = rhs.pRootSignature;
}

GraphicPso& GraphicPso::operator=(const GraphicPso& rhs)
{
    pRootSignature = rhs.pRootSignature;
    mPsoDesc = rhs.mPsoDesc;
    return *this;
}

GraphicPso& GraphicPso::operator=(const GraphicPso&& rhs)
{
    pRootSignature = std::move(rhs.pRootSignature);
    mPsoDesc = std::move(rhs.mPsoDesc);
    return *this;
}

void GraphicPso::SetBlendState(const D3D12_BLEND_DESC& BlendDesc)
{
    mPsoDesc.BlendState = BlendDesc;
}

void GraphicPso::SetRasterizerState(const D3D12_RASTERIZER_DESC& RasterizerDesc)
{
    mPsoDesc.RasterizerState = RasterizerDesc;
}

void GraphicPso::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc)
{
    mPsoDesc.DepthStencilState = DepthStencilDesc;
}

void GraphicPso::SetSampleMask(UINT SampleMask)
{
    mPsoDesc.SampleMask = SampleMask;
}

void GraphicPso::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType)
{
    assert(TopologyType != D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED, "Can't draw with undefined topology");
    mPsoDesc.PrimitiveTopologyType = TopologyType;
}

void GraphicPso::SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBProps)
{
    mPsoDesc.IBStripCutValue = IBProps;
}

void GraphicPso::SetFillMode(D3D12_FILL_MODE fillMode)
{
    mPsoDesc.RasterizerState.FillMode = fillMode;
}

void GraphicPso::SetNumberRenderTargets(UINT num)
{
    mPsoDesc.NumRenderTargets = num;
}

void GraphicPso::SetDepthTargetFormat(DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality)
{
    SetRenderTargetFormats(0, nullptr, DSVFormat, MsaaCount, MsaaQuality);
}

void GraphicPso::SetRenderTargetFormat(DXGI_FORMAT RTVFormat, DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality)
{
    SetRenderTargetFormats(1, &RTVFormat, DSVFormat, MsaaCount, MsaaQuality);
}

void GraphicPso::SetRenderTargetFormats(UINT NumRTVs, const DXGI_FORMAT* RTVFormats, DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality)
{
    assert(NumRTVs == 0 || RTVFormats != nullptr, "Null format array conflicts with non-zero length");
    for (UINT i = 0; i < NumRTVs; ++i)
    {
        assert(RTVFormats[i] != DXGI_FORMAT_UNKNOWN);
        mPsoDesc.RTVFormats[i] = RTVFormats[i];
    }
    for (UINT i = NumRTVs; i < mPsoDesc.NumRenderTargets; ++i)
        mPsoDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
    mPsoDesc.NumRenderTargets = NumRTVs;
    mPsoDesc.DSVFormat = DSVFormat;
    mPsoDesc.SampleDesc.Count = MsaaCount;
    mPsoDesc.SampleDesc.Quality = MsaaQuality;
}

void GraphicPso::Finalize(std::wstring name, const InputLayout* inputLayouts)
{
    mPsoDesc.pRootSignature = pRootSignature->GetSignature();
    assert(mPsoDesc.pRootSignature);
    mPsoDesc.InputLayout = inputLayouts->GetInputLayoutDescriptor();
    assert(mPsoDesc.DepthStencilState.DepthEnable != (mPsoDesc.DSVFormat == DXGI_FORMAT_UNKNOWN));
    Device::GetDevice()->CreateGraphicsPipelineState(&mPsoDesc, IID_PPV_ARGS(&mPso)) >> Kds::App::Check;
    mPso->SetName(name.c_str());
}

void ComputePso::Finalize(std::wstring name)
{
    // Make sure the root signature is finalized first
    mPsoDesc.pRootSignature = pRootSignature->GetSignature();
    assert(mPsoDesc.pRootSignature);

    Device::GetDevice()->CreateComputePipelineState(&mPsoDesc, IID_PPV_ARGS(&mPso)) >> Kds::App::Check;
    mPso->SetName(name.c_str());
}

ComputePso::ComputePso()
{
    ZeroMemory(&mPsoDesc, sizeof(mPsoDesc));
    mPsoDesc.NodeMask = 1;
}
