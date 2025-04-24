#include "RootSignature.h"
#include "D3D12Utils.h"
#include "Device.h"


using Microsoft::WRL::ComPtr;


void RootSignature::Finalize(const std::wstring& name, D3D12_ROOT_SIGNATURE_FLAGS Flags)
{
    if (bFinalized) return;
    auto staticSamplers = GetStaticSamplers();

    D3D12_ROOT_SIGNATURE_DESC RootDesc;
    RootDesc.NumParameters = mNumParameters;
    RootDesc.pParameters = (const D3D12_ROOT_PARAMETER*)mParamArray.get();
    RootDesc.NumStaticSamplers = (UINT)staticSamplers.size();
    RootDesc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)staticSamplers.data();
    RootDesc.Flags = Flags;

    ID3D12RootSignature** RSRef = nullptr;

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(5, mParamArray.get(), (UINT)staticSamplers.size(), staticSamplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    hr >> Kds::App::Check;

    Device::GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf())) >> Kds::App::Check;

    bFinalized = true;
}
