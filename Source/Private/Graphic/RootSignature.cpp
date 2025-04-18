#include "RootSignature.h"
#include <map>
#include <thread>
#include <mutex>


using Microsoft::WRL::ComPtr;


void RootSignature::InitStaticSampler(UINT Register, const D3D12_SAMPLER_DESC& NonStaticSamplerDesc, D3D12_SHADER_VISIBILITY Visibility)
{
    assert(mNumInitializedStaticSamplers < mNumSamplers);
    D3D12_STATIC_SAMPLER_DESC& StaticSamplerDesc = mSamplerArray[mNumInitializedStaticSamplers++];

    StaticSamplerDesc.Filter = NonStaticSamplerDesc.Filter;
    StaticSamplerDesc.AddressU = NonStaticSamplerDesc.AddressU;
    StaticSamplerDesc.AddressV = NonStaticSamplerDesc.AddressV;
    StaticSamplerDesc.AddressW = NonStaticSamplerDesc.AddressW;
    StaticSamplerDesc.MipLODBias = NonStaticSamplerDesc.MipLODBias;
    StaticSamplerDesc.MaxAnisotropy = NonStaticSamplerDesc.MaxAnisotropy;
    StaticSamplerDesc.ComparisonFunc = NonStaticSamplerDesc.ComparisonFunc;
    StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
    StaticSamplerDesc.MinLOD = NonStaticSamplerDesc.MinLOD;
    StaticSamplerDesc.MaxLOD = NonStaticSamplerDesc.MaxLOD;
    StaticSamplerDesc.ShaderRegister = Register;
    StaticSamplerDesc.RegisterSpace = 0;
    StaticSamplerDesc.ShaderVisibility = Visibility;

    if (StaticSamplerDesc.AddressU == D3D12_TEXTURE_ADDRESS_MODE_BORDER || StaticSamplerDesc.AddressV == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
        StaticSamplerDesc.AddressW == D3D12_TEXTURE_ADDRESS_MODE_BORDER)
    {
        WARN_ONCE_IF_NOT(
            // Transparent Black
            NonStaticSamplerDesc.BorderColor[0] == 0.0f && NonStaticSamplerDesc.BorderColor[1] == 0.0f && NonStaticSamplerDesc.BorderColor[2] == 0.0f &&
                    NonStaticSamplerDesc.BorderColor[3] == 0.0f ||
                // Opaque Black
                NonStaticSamplerDesc.BorderColor[0] == 0.0f && NonStaticSamplerDesc.BorderColor[1] == 0.0f && NonStaticSamplerDesc.BorderColor[2] == 0.0f &&
                    NonStaticSamplerDesc.BorderColor[3] == 1.0f ||
                // Opaque White
                NonStaticSamplerDesc.BorderColor[0] == 1.0f && NonStaticSamplerDesc.BorderColor[1] == 1.0f && NonStaticSamplerDesc.BorderColor[2] == 1.0f &&
                    NonStaticSamplerDesc.BorderColor[3] == 1.0f,
            "Sampler border color does not match static sampler limitations");

        if (NonStaticSamplerDesc.BorderColor[3] == 1.0f)
        {
            if (NonStaticSamplerDesc.BorderColor[0] == 1.0f)
                StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
            else
                StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
        }
        else
            StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    }
}

void RootSignature::Finalize(const std::wstring& name, D3D12_ROOT_SIGNATURE_FLAGS Flags)
{
    if (bFinalized) return;

    assert(mNumInitializedStaticSamplers == mNumSamplers);

    D3D12_ROOT_SIGNATURE_DESC RootDesc;
    RootDesc.NumParameters = mNumParameters;
    RootDesc.pParameters = (const D3D12_ROOT_PARAMETER*)mParamArray.get();
    RootDesc.NumStaticSamplers = mNumSamplers;
    RootDesc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)mSamplerArray.get();
    RootDesc.Flags = Flags;

    mDescriptorTableBitMap = 0;
    mSamplerTableBitMap = 0;

    size_t HashCode = Utility::HashState(&RootDesc.Flags);
    HashCode = Utility::HashState(RootDesc.pStaticSamplers, mNumSamplers, HashCode);

    for (UINT Param = 0; Param < mNumParameters; ++Param)
    {
        const D3D12_ROOT_PARAMETER& RootParam = RootDesc.pParameters[Param];
        mDescriptorTableSize[Param] = 0;

        if (RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
            assert(RootParam.DescriptorTable.pDescriptorRanges != nullptr);

            HashCode = Utility::HashState(RootParam.DescriptorTable.pDescriptorRanges, RootParam.DescriptorTable.NumDescriptorRanges, HashCode);

            // We keep track of sampler descriptor tables separately from CBV_SRV_UAV descriptor tables
            if (RootParam.DescriptorTable.pDescriptorRanges->RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
                mSamplerTableBitMap |= (1 << Param);
            else
                mDescriptorTableBitMap |= (1 << Param);

            for (UINT TableRange = 0; TableRange < RootParam.DescriptorTable.NumDescriptorRanges; ++TableRange)
                mDescriptorTableSize[Param] += RootParam.DescriptorTable.pDescriptorRanges[TableRange].NumDescriptors;
        }
        else
            HashCode = Utility::HashState(&RootParam, 1, HashCode);
    }

    ID3D12RootSignature** RSRef = nullptr;
    bool firstCompile = false;
    {
        static mutex s_HashMapMutex;
        lock_guard<mutex> CS(s_HashMapMutex);
        auto iter = sRootSignatureHashMap.find(HashCode);

        // Reserve space so the next inquiry will find that someone got here first.
        if (iter == s_RootSignatureHashMap.end())
        {
            RSRef = s_RootSignatureHashMap[HashCode].GetAddressOf();
            firstCompile = true;
        }
        else
            RSRef = iter->second.GetAddressOf();
    }

    if (firstCompile)
    {
        ComPtr<ID3DBlob> pOutBlob, pErrorBlob;

        ASSERT_SUCCEEDED(D3D12SerializeRootSignature(&RootDesc, D3D_ROOT_SIGNATURE_VERSION_1, pOutBlob.GetAddressOf(), pErrorBlob.GetAddressOf()));

        ASSERT_SUCCEEDED(g_Device->CreateRootSignature(1, pOutBlob->GetBufferPointer(), pOutBlob->GetBufferSize(), MY_IID_PPV_ARGS(&m_Signature)));

        m_Signature->SetName(name.c_str());

        s_RootSignatureHashMap[HashCode].Attach(m_Signature);
        ASSERT(*RSRef == m_Signature);
    }
    else
    {
        while (*RSRef == nullptr)
            this_thread::yield();
        mSignature = *RSRef;
    }

    bFinalized = true;
}
