#include "RootSignature.h"
#include "D3D12Utils.h"

RootParameter::RootParameter()
{
    mRootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
}

RootParameter::~RootParameter()
{
    Clear();
}

void RootParameter::Clear() noexcept
{
    if (mRootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
    {
        delete[] mRootParam.DescriptorTable.pDescriptorRanges;
    }
    mRootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
}

void RootParameter::InitAsConstants(UINT Register, UINT NumDwords, D3D12_SHADER_VISIBILITY Visibility, UINT Space) noexcept
{
    mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    mRootParam.ShaderVisibility = Visibility;
    mRootParam.Constants.Num32BitValues = NumDwords;
    mRootParam.Constants.ShaderRegister = Register;
    mRootParam.Constants.RegisterSpace = Space;
}

void RootParameter::InitAsConstantBuffer(UINT Register, D3D12_SHADER_VISIBILITY Visibility, UINT Space) noexcept
{
    mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    mRootParam.ShaderVisibility = Visibility;
    mRootParam.Descriptor.ShaderRegister = Register;
    mRootParam.Descriptor.RegisterSpace = Space;
}

void RootParameter::InitAsBufferSRV(UINT Register, D3D12_SHADER_VISIBILITY Visibility, UINT Space) noexcept
{
    mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    mRootParam.ShaderVisibility = Visibility;
    mRootParam.Descriptor.ShaderRegister = Register;
    mRootParam.Descriptor.RegisterSpace = Space;
}

void RootParameter::InitAsBufferUAV(UINT Register, D3D12_SHADER_VISIBILITY Visibility, UINT Space) noexcept
{
    mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
    mRootParam.ShaderVisibility = Visibility;
    mRootParam.Descriptor.ShaderRegister = Register;
    mRootParam.Descriptor.RegisterSpace = Space;
}

void RootParameter::InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Register, UINT Count, D3D12_SHADER_VISIBILITY Visibility, UINT Space) noexcept
{
    InitAsDescriptorTable(1, Visibility);
    SetTableRange(0, Type, Register, Count, Space);
}

void RootParameter::InitAsDescriptorTable(UINT RangeCount, D3D12_SHADER_VISIBILITY Visibility)
{
    mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    mRootParam.ShaderVisibility = Visibility;
    mRootParam.DescriptorTable.NumDescriptorRanges = RangeCount;
    mRootParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[RangeCount];
}

void RootParameter::SetTableRange(UINT RangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Register, UINT Count, UINT Space)
{
    D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(mRootParam.DescriptorTable.pDescriptorRanges + RangeIndex);
    range->RangeType = Type;
    range->NumDescriptors = Count;
    range->BaseShaderRegister = Register;
    range->RegisterSpace = Space;
    range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}



RootSignature::RootSignature(const std::wstring& name, UINT size) : mName(name)
{
    for (size_t i = 0; i < size; i++)
    {
        mParams.emplace_back();
    }
}

RootSignature::~RootSignature() {}

RootParameter* RootSignature::GetParam(size_t entryIndex)
{
    if (IsIndexValid(entryIndex))
    {
        return &mParams[entryIndex];
    }
    return nullptr;
}

const RootParameter* RootSignature::GetParam(size_t entryIndex) const
{
    if (IsIndexValid(entryIndex))
    {
        return &mParams[entryIndex];
    }
    return nullptr;
}

void RootSignature::Finalize(D3D12_ROOT_SIGNATURE_FLAGS Flags)
{
    if (bFinalized) return;

    auto staticSamplers = GetStaticSamplers();

    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc((UINT)mParams.size(), (const D3D12_ROOT_PARAMETER*)mParams.data(), (UINT)staticSamplers.size(), staticSamplers.data(), Flags);

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

bool RootSignature::IsIndexValid(size_t entryIndex) const noexcept
{
    assert(entryIndex < mParams.size());
    if (entryIndex >= mParams.size())
    {
        LOG_ERROR("Index is out of range");
        return false;
    }
    return true;
}

