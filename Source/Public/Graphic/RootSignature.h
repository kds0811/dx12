#pragma once
#include "GraphicCommonHeaders.h"
#include <vector>
#include "Device.h"
#include "Logger.h"

class RootParameter
{
    D3D12_ROOT_PARAMETER mRootParam;

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
    }

    inline void InitAsConstants(UINT Register, UINT NumDwords, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept
    {

        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.Constants.Num32BitValues = NumDwords;
        mRootParam.Constants.ShaderRegister = Register;
        mRootParam.Constants.RegisterSpace = Space;
    }

    inline void InitAsConstantBuffer(UINT Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept
    {
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.Descriptor.ShaderRegister = Register;
        mRootParam.Descriptor.RegisterSpace = Space;
    }

    inline void InitAsBufferSRV(UINT Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept
    {
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.Descriptor.ShaderRegister = Register;
        mRootParam.Descriptor.RegisterSpace = Space;
    }

    inline void InitAsBufferUAV(UINT Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept
    {
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.Descriptor.ShaderRegister = Register;
        mRootParam.Descriptor.RegisterSpace = Space;
    }

    inline void InitAsDescriptorRange(
        D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Register, UINT Count, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept
    {
        InitAsDescriptorTable(1, Visibility);
        SetTableRange(0, Type, Register, Count, Space);
    }

    [[nodiscard]] inline const D3D12_ROOT_PARAMETER& Get() const noexcept { return mRootParam; }


private:
    inline void InitAsDescriptorTable(UINT RangeCount, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL)
    {
        mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        mRootParam.ShaderVisibility = Visibility;
        mRootParam.DescriptorTable.NumDescriptorRanges = RangeCount;
        mRootParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[RangeCount];
    }

    inline void SetTableRange(UINT RangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Register, UINT Count, UINT Space = 0)
    {
        D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(mRootParam.DescriptorTable.pDescriptorRanges + RangeIndex);
        range->RangeType = Type;
        range->NumDescriptors = Count;
        range->BaseShaderRegister = Register;
        range->RegisterSpace = Space;
        range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    }
};


class RootSignature
{
    bool bFinalized = false;
    std::vector<RootParameter> mParams;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
    std::wstring mName{};

public:
    RootSignature(const std::wstring& name) : mName(name) {}

    ~RootSignature() {}

    inline RootParameter* GetParam(size_t entryIndex)
    {
        if (IsIndexValid(entryIndex))
        {
            return &mParams[entryIndex];
        }
        return nullptr;
    }

    inline const RootParameter* GetParam(size_t entryIndex) const
    {
        if (IsIndexValid(entryIndex))
        {
            return &mParams[entryIndex];
        }
        return nullptr;
    }

    void Finalize(D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

    [[nodiscard]] inline ID3D12RootSignature* GetSignature() const { return mRootSignature.Get(); }

    [[nodiscard]] inline bool IsIndexValid(size_t entryIndex) const noexcept
    {
        assert(entryIndex < mParams.size());
        if (entryIndex >= mParams.size())
        {
            LOG_ERROR("Index is out of range");
            return false;
        }
        return true;
    }
};

