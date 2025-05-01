#include "RootSignatureManager.h"
#include "RootSignature.h"

RootSignatureManager::RootSignatureManager()
{
    BuildRootSignatures();
}

RootSignatureManager::~RootSignatureManager() = default;

void RootSignatureManager::BuildRootSignatures()
{
    BuildBaseRootSignature();
    BuildPostProcessRootSignature();
    BuildBilateralBlurRootSignature();
}

void RootSignatureManager::BuildBaseRootSignature()
{
    std::wstring name = L"base";
    std::unique_ptr<RootSignature> baseRootSig = std::make_unique<RootSignature>(name, 5);
    baseRootSig->GetParam(0)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
    baseRootSig->GetParam(1)->InitAsConstantBuffer(0);
    baseRootSig->GetParam(2)->InitAsConstantBuffer(1);
    baseRootSig->GetParam(3)->InitAsConstantBuffer(2);
    baseRootSig->GetParam(4)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, D3D12_SHADER_VISIBILITY_PIXEL);
    baseRootSig->Finalize();
    mRootSignatures[name] = std::move(baseRootSig);
}

void RootSignatureManager::BuildPostProcessRootSignature()
{
    std::wstring name = L"postProcess";
    std::unique_ptr<RootSignature> postprocRootSig = std::make_unique<RootSignature>(name, 5);
    postprocRootSig->GetParam(0)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
    postprocRootSig->GetParam(1)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
    postprocRootSig->GetParam(2)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
    postprocRootSig->Finalize();
    mRootSignatures[name] = std::move(postprocRootSig);
}

void RootSignatureManager::BuildBilateralBlurRootSignature()
{
    std::wstring name = L"bilateral";
    std::unique_ptr<RootSignature> bilateralRootSig = std::make_unique<RootSignature>(name, 5);
    bilateralRootSig->GetParam(0)->InitAsConstants(16, 0);
    bilateralRootSig->GetParam(1)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
    bilateralRootSig->GetParam(2)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
    bilateralRootSig->Finalize();
    mRootSignatures[name] = std::move(bilateralRootSig);
}
