#pragma once
#include "GraphicCommonHeaders.h"
#include <vector>
#include "Device.h"
#include "Logger.h"

class RootParameter
{
    D3D12_ROOT_PARAMETER mRootParam;

public:
    RootParameter();
    ~RootParameter();

    void Clear() noexcept;
    void InitAsConstants(UINT Register, UINT NumDwords, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept;
    void InitAsConstantBuffer(UINT Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept;
    void InitAsBufferSRV(UINT Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept;
    void InitAsBufferUAV(UINT Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept;
    void InitAsDescriptorRange(
        D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Register, UINT Count, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, UINT Space = 0) noexcept;
    [[nodiscard]] inline const D3D12_ROOT_PARAMETER& Get() const noexcept { return mRootParam; }

private:
    void InitAsDescriptorTable(UINT RangeCount, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);
    void SetTableRange(UINT RangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Register, UINT Count, UINT Space = 0);
};


class RootSignature
{
    bool bFinalized = false;
    std::vector<RootParameter> mParams;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
    std::wstring mName{};

public:
    RootSignature(const std::wstring& name, UINT size);
    ~RootSignature();

    RootParameter* GetParam(size_t entryIndex);
    const RootParameter* GetParam(size_t entryIndex) const;
    void Finalize(D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);
    [[nodiscard]] inline ID3D12RootSignature* GetSignature() const { return mRootSignature.Get(); }
    [[nodiscard]] bool IsIndexValid(size_t entryIndex) const noexcept;

};

