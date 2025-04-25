#pragma once
#include "GraphicCommonHeaders.h"
#include <array>
#include "Device.h"
#include "Logger.h"

class RootParameter
{
    D3D12_ROOT_PARAMETER mRootParam;
    bool bIsInitialize = false;

public:
    RootParameter() { mRootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF; }

    ~RootParameter() { Clear(); }

    inline void Clear() noexcept
    {
        if (mRootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
            delete[] mRootParam.DescriptorTable.pDescriptorRanges;
        }
        mRootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
        bIsInitialize = false;
    }

    inline void InitAsConstants(UINT Register, UINT NumDwords, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept
    {
        if (IsInitialize()) return;
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.Constants.Num32BitValues = NumDwords;
        mRootParam.Constants.ShaderRegister = Register;
        mRootParam.Constants.RegisterSpace = Space;
        bIsInitialize = true;
    }

    inline void InitAsConstantBuffer(UINT Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept
    {
        if (IsInitialize()) return;
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.Descriptor.ShaderRegister = Register;
        mRootParam.Descriptor.RegisterSpace = Space;
        bIsInitialize = true;
    }

    inline void InitAsBufferSRV(UINT Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept
    {
        if (IsInitialize()) return;
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.Descriptor.ShaderRegister = Register;
        mRootParam.Descriptor.RegisterSpace = Space;
        bIsInitialize = true;
    }

    inline void InitAsBufferUAV(UINT Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept
    {
        if (IsInitialize()) return;
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.Descriptor.ShaderRegister = Register;
        mRootParam.Descriptor.RegisterSpace = Space;
        bIsInitialize = true;
    }

    inline void InitAsDescriptorRange(
        D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Register, UINT Count, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept
    {
        if (IsInitialize()) return;
        InitAsDescriptorTable(1, Visibility);
        SetTableRange(0, Type, Register, Count, Space);
        bIsInitialize = true;
    }

    inline void InitAsDescriptorTable(UINT RangeCount, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL) noexcept
    {
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.DescriptorTable.NumDescriptorRanges = RangeCount;
        mRootParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[RangeCount];
    }

    inline void SetTableRange(UINT RangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Register, UINT Count, UINT Space = 0) noexcept
    {
        D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(mRootParam.DescriptorTable.pDescriptorRanges + RangeIndex);
        range->RangeType = Type;
        range->NumDescriptors = Count;
        range->BaseShaderRegister = Register;
        range->RegisterSpace = Space;
        range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    }

    [[nodiscard]] inline const D3D12_ROOT_PARAMETER& Get() const noexcept { return mRootParam; }

    inline bool IsInitialize() const noexcept
    {
        if (bIsInitialize)
        {
            LOG_WARNING("Root Param allready os initialized");
            return true;
        }
        return false;
    }
};

template <size_t size>
class RootSignature
{
    bool bFinalized = false;
    static constexpr size_t mNumParameters = size;
    std::array<RootParameter, mNumParameters> mParamArray;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
    std::wstring mName{};

public:
    RootSignature(const std::wstring& name) : mName(name) { static_assert(mNumParameters < 10); }

    ~RootSignature() {}

    inline RootParameter* operator[](size_t entryIndex)
    {
        if (IsIndexValid(entryIndex))
        {
            return &mParamArray[entryIndex];
        }
        return nullptr;
    }

    inline const RootParameter* operator[](size_t entryIndex) const
    {
        if (IsIndexValid(entryIndex))
        {
            return &mParamArray[entryIndex];
        }
        return nullptr;
    }

    void Finalize(D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

    [[nodiscard]] inline ID3D12RootSignature* GetSignature() const { return mRootSignature.Get(); }

    [[nodiscard]] inline bool IsIndexValid(size_t entryIndex) const noexcept
    {
        assert(entryIndex < mNumParameters);
        if (entryIndex => mNumParameters)
        {
            LOG_ERROR("Index is out of range");
            return false;
        }
        return true;
    }
};

template <size_t size>
inline void RootSignature<size>::Finalize(D3D12_ROOT_SIGNATURE_FLAGS Flags)
{
    if (bFinalized) return;

    auto staticSamplers = GetStaticSamplers();

    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(mNumParameters, mParamArray.data(), (UINT)staticSamplers.size(), staticSamplers.data(), Flags);

    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    hr >> Kds::App::Check;

    Device::GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf())) >>
        Kds::App::Check;

    bFinalized = true;
}
