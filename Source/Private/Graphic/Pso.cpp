#include "Pso.h"
#include "RootSignature.h"
#include "Device.h"
#include "InputLayout.h"

Pso::Pso() = default;

Pso::Pso(ID3D12PipelineState* pso, const RootSignature* const rootSignature) : mPso(pso), pRootSignature(rootSignature) {}

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

GraphicsPso::GraphicsPso(const RootSignature* rootSignature, const InputLayout* inputLayouts) : pInputLayouts(inputLayouts)
{
    SetRootSignature(rootSignature);
    ZeroMemory(&mPsoDesc, sizeof(mPsoDesc));
    mPsoDesc.NodeMask = 1;
    mPsoDesc.SampleMask = 0xFFFFFFFFu;
    mPsoDesc.SampleDesc.Count = 1;
    mPsoDesc.InputLayout.NumElements = 0;
}

GraphicsPso::GraphicsPso(const GraphicsPso& rhs)
    :
    Pso(rhs.mPso.Get(), rhs.pRootSignature),
    mPsoDesc(rhs.mPsoDesc),
    pInputLayouts(rhs.pInputLayouts)
{}

GraphicsPso::GraphicsPso(const GraphicsPso&& rhs) 
    :
    Pso(std::move(rhs.mPso.Get()), std::move(rhs.pRootSignature)),
    mPsoDesc(std::move(rhs.mPsoDesc)),
    pInputLayouts(std::move(rhs.pInputLayouts))
{}

GraphicsPso& GraphicsPso::operator=(const GraphicsPso& rhs)
{
    mPso = rhs.mPso;
    pRootSignature = rhs.pRootSignature;
    mPsoDesc = rhs.mPsoDesc;
    pInputLayouts = rhs.pInputLayouts;
    return *this;
}

GraphicsPso& GraphicsPso::operator=(const GraphicsPso&& rhs)
{
    mPso = std::move(rhs.mPso);
    pRootSignature = std::move(rhs.pRootSignature);
    mPsoDesc = std::move(rhs.mPsoDesc);
    pInputLayouts = std::move(rhs.pInputLayouts);
    return *this;
}

void GraphicsPso::SetBlendState(const D3D12_BLEND_DESC& BlendDesc)
{
    mPsoDesc.BlendState = BlendDesc;
}

void GraphicsPso::SetRasterizerState(const D3D12_RASTERIZER_DESC& RasterizerDesc)
{
    mPsoDesc.RasterizerState = RasterizerDesc;
}

void GraphicsPso::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc)
{
    mPsoDesc.DepthStencilState = DepthStencilDesc;
}

void GraphicsPso::SetSampleMask(UINT SampleMask)
{
    mPsoDesc.SampleMask = SampleMask;
}

void GraphicsPso::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType)
{
    assert(TopologyType != D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED, "Can't draw with undefined topology");
    mPsoDesc.PrimitiveTopologyType = TopologyType;
}

void GraphicsPso::SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBProps)
{
    mPsoDesc.IBStripCutValue = IBProps;
}

void GraphicsPso::SetDepthTargetFormat(DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality)
{
    SetRenderTargetFormats(0, nullptr, DSVFormat, MsaaCount, MsaaQuality);
}

void GraphicsPso::SetRenderTargetFormat(DXGI_FORMAT RTVFormat, DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality)
{
    SetRenderTargetFormats(1, &RTVFormat, DSVFormat, MsaaCount, MsaaQuality);
}

void GraphicsPso::SetRenderTargetFormats(UINT NumRTVs, const DXGI_FORMAT* RTVFormats, DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality)
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



void GraphicsPso::Finalize(std::wstring name)
{
    mPsoDesc.pRootSignature = pRootSignature->GetSignature();
    assert(mPsoDesc.pRootSignature);

    mPsoDesc.InputLayout =  mInputLayouts->GetInputLayoutDescriptor();

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
