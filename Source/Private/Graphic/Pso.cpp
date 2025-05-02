#include "Pso.h"
#include "RootSignature.h"


Pso::Pso(const std::wstring& name) : mName(name) {}

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

GraphicsPso::GraphicsPso(const wchar_t* Name) : Pso(Name)
{
    ZeroMemory(&mPsoDesc, sizeof(mPsoDesc));
    mPsoDesc.NodeMask = 1;
    mPsoDesc.SampleMask = 0xFFFFFFFFu;
    mPsoDesc.SampleDesc.Count = 1;
    mPsoDesc.InputLayout.NumElements = 0;
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

void GraphicsPso::SetInputLayout(UINT NumElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs)
{
    mPsoDesc.InputLayout.NumElements = NumElements;

    if (NumElements > 0)
    {
        D3D12_INPUT_ELEMENT_DESC* NewElements = (D3D12_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D12_INPUT_ELEMENT_DESC) * NumElements);
        memcpy(NewElements, pInputElementDescs, NumElements * sizeof(D3D12_INPUT_ELEMENT_DESC));
        mInputLayouts.reset((const D3D12_INPUT_ELEMENT_DESC*)NewElements);
    }
    else
        mInputLayouts = nullptr;
}

void GraphicsPso::Finalize()
{
    // Make sure the root signature is finalized first
    mPsoDesc.pRootSignature = pRootSignature->GetSignature();
    assert(mPsoDesc.pRootSignature != nullptr);

    mPsoDesc.InputLayout.pInputElementDescs = nullptr;
    size_t HashCode = Utility::HashState(&mPsoDesc);
    HashCode = Utility::HashState(m_InputLayouts.get(), mPsoDesc.InputLayout.NumElements, HashCode);
    mPsoDesc.InputLayout.pInputElementDescs = m_InputLayouts.get();

    ID3D12PipelineState** PSORef = nullptr;
    bool firstCompile = false;
    {
        static mutex s_HashMapMutex;
        lock_guard<mutex> CS(s_HashMapMutex);
        auto iter = s_GraphicsPSOHashMap.find(HashCode);

        // Reserve space so the next inquiry will find that someone got here first.
        if (iter == s_GraphicsPSOHashMap.end())
        {
            firstCompile = true;
            PSORef = s_GraphicsPSOHashMap[HashCode].GetAddressOf();
        }
        else
            PSORef = iter->second.GetAddressOf();
    }

    if (firstCompile)
    {
        assert(mPsoDesc.DepthStencilState.DepthEnable != (mPsoDesc.DSVFormat == DXGI_FORMAT_UNKNOWN));
        ASSERT_SUCCEEDED(g_Device->CreateGraphicsPipelineState(&mPsoDesc, MY_IID_PPV_ARGS(&m_PSO)));
        s_GraphicsPSOHashMap[HashCode].Attach(m_PSO);
        m_PSO->SetName(m_Name);
    }
    else
    {
        while (*PSORef == nullptr)
            this_thread::yield();
        m_PSO = *PSORef;
    }
}

void ComputePSO::Finalize()
{
    // Make sure the root signature is finalized first
    mPsoDesc.pRootSignature = mRootSignature->GetSignature();
    assert(mPsoDesc.pRootSignature != nullptr);

    size_t HashCode = Utility::HashState(&mPsoDesc);

    ID3D12PipelineState** PSORef = nullptr;
    bool firstCompile = false;
    {
        static mutex s_HashMapMutex;
        lock_guard<mutex> CS(s_HashMapMutex);
        auto iter = s_ComputePSOHashMap.find(HashCode);

        // Reserve space so the next inquiry will find that someone got here first.
        if (iter == s_ComputePSOHashMap.end())
        {
            firstCompile = true;
            PSORef = s_ComputePSOHashMap[HashCode].GetAddressOf();
        }
        else
            PSORef = iter->second.GetAddressOf();
    }

    if (firstCompile)
    {
        ASSERT_SUCCEEDED(g_Device->CreateComputePipelineState(&mPsoDesc, MY_IID_PPV_ARGS(&m_PSO)));
        s_ComputePSOHashMap[HashCode].Attach(m_PSO);
        m_PSO->SetName(m_Name);
    }
    else
    {
        while (*PSORef == nullptr)
            this_thread::yield();
        m_PSO = *PSORef;
    }
}

ComputePso::ComputePso(const wchar_t* Name) : Pso(Name)
{
    ZeroMemory(&mPsoDesc, sizeof(mPsoDesc));
    mPsoDesc.NodeMask = 1;
}



