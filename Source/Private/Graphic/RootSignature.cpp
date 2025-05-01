#include "RootSignature.h"
#include "D3D12Utils.h"

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
