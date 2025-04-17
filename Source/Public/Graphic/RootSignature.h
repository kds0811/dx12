#pragma once
#include "GraphicCommonHeaders.h"

class RootParameter
{
    friend class RootSignature;

protected:
    D3D12_ROOT_PARAMETER mRootParam;

public:
    RootParameter() { mRootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF; }

    ~RootParameter() { Clear(); }

    void Clear()
    {
        if (mRootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
            delete[] mRootParam.DescriptorTable.pDescriptorRanges;
        }
        mRootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
    }

    void InitAsConstants(UINT Register, UINT NumDwords, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0)
    {
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.Constants.Num32BitValues = NumDwords;
        mRootParam.Constants.ShaderRegister = Register;
        mRootParam.Constants.RegisterSpace = Space;
    }

    void InitAsConstantBuffer(UINT Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0)
    {
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.Descriptor.ShaderRegister = Register;
        mRootParam.Descriptor.RegisterSpace = Space;
    }

    void InitAsBufferSRV(UINT Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0)
    {
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.Descriptor.ShaderRegister = Register;
        mRootParam.Descriptor.RegisterSpace = Space;
    }

    void InitAsBufferUAV(UINT Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0)
    {
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.Descriptor.ShaderRegister = Register;
        mRootParam.Descriptor.RegisterSpace = Space;
    }

    void InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Register, UINT Count, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0)
    {
        InitAsDescriptorTable(1, Visibility);
        SetTableRange(0, Type, Register, Count, Space);
    }

    void InitAsDescriptorTable(UINT RangeCount, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL)
    {
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.DescriptorTable.NumDescriptorRanges = RangeCount;
        mRootParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[RangeCount];
    }

    void SetTableRange(UINT RangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Register, UINT Count, UINT Space = 0)
    {
        D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(mRootParam.DescriptorTable.pDescriptorRanges + RangeIndex);
        range->RangeType = Type;
        range->NumDescriptors = Count;
        range->BaseShaderRegister = Register;
        range->RegisterSpace = Space;
        range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    }

    const D3D12_ROOT_PARAMETER& operator()(void) const { return mRootParam; }


};

// Maximum 64 DWORDS divied up amongst all root parameters.
// Root constants = 1 DWORD * NumConstants
// Root descriptor (CBV, SRV, or UAV) = 2 DWORDs each
// Descriptor table pointer = 1 DWORD
// Static samplers = 0 DWORDS (compiled into shader)
class RootSignature
{

public:
    RootSignature(UINT NumRootParams = 0, UINT NumStaticSamplers = 0) : bFinalized(false), mNumParameters(NumRootParams) { Reset(NumRootParams, NumStaticSamplers); }

    ~RootSignature() {}

    static void DestroyAll(void);

    void Reset(UINT NumRootParams, UINT NumStaticSamplers = 0)
    {
        if (NumRootParams > 0)
            mParamArray.reset(new RootParameter[NumRootParams]);
        else
            mParamArray = nullptr;
        mNumParameters = NumRootParams;

        if (NumStaticSamplers > 0)
            mSamplerArray.reset(new D3D12_STATIC_SAMPLER_DESC[NumStaticSamplers]);
        else
            mSamplerArray = nullptr;
        mNumSamplers = NumStaticSamplers;
        mNumInitializedStaticSamplers = 0;
    }

    RootParameter& operator[](size_t EntryIndex)
    {
        assert(EntryIndex < mNumParameters);
        return mParamArray.get()[EntryIndex];
    }

    const RootParameter& operator[](size_t EntryIndex) const
    {
        assert(EntryIndex < mNumParameters);
        return mParamArray.get()[EntryIndex];
    }

    void InitStaticSampler(UINT Register, const D3D12_SAMPLER_DESC& NonStaticSamplerDesc, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);

    void Finalize(const std::wstring& name, D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

    ID3D12RootSignature* GetSignature() const { return mSignature; }

protected:
    bool bFinalized;
    UINT mNumParameters;
    UINT mNumSamplers;
    UINT mNumInitializedStaticSamplers;
    uint32_t mDescriptorTableBitMap;    // One bit is set for root parameters that are non-sampler descriptor tables
    uint32_t mSamplerTableBitMap;       // One bit is set for root parameters that are sampler descriptor tables
    uint32_t mDescriptorTableSize[16];  // Non-sampler descriptor tables need to know their descriptor count
    std::unique_ptr<RootParameter[]> mParamArray;
    std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> mSamplerArray;
    ID3D12RootSignature* mSignature;
};
