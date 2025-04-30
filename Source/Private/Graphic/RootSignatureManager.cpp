#include "RootSignatureManager.h"

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
    mStandartRootSignature = std::make_unique<RootSignature<mStandrartRootSigSize>>();
    mStandartRootSignature->GetParam(0)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
    mStandartRootSignature->GetParam(1)->InitAsConstantBuffer(0);
    mStandartRootSignature->GetParam(2)->InitAsConstantBuffer(1);
    mStandartRootSignature->GetParam(3)->InitAsConstantBuffer(2);
    mStandartRootSignature->GetParam(4)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, D3D12_SHADER_VISIBILITY_PIXEL);
    mStandartRootSignature->Finalize();
}

void RootSignatureManager::BuildPostProcessRootSignature()
{
    mPostProcRootSignature = std::make_unique<RootSignature<mPostProcRootSigSize>>();
    mPostProcRootSignature->GetParam(0)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
    mPostProcRootSignature->GetParam(1)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
    mPostProcRootSignature->GetParam(2)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
    mPostProcRootSignature->Finalize();
}

void RootSignatureManager::BuildBilateralBlurRootSignature()
{
    mBilateralBlurRootSignature = std::make_unique<RootSignature<mBilateralBlurRootSigSize>>();
    mBilateralBlurRootSignature->GetParam(0)->InitAsConstants(16, 0);
    mBilateralBlurRootSignature->GetParam(1)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
    mBilateralBlurRootSignature->GetParam(2)->InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
    mBilateralBlurRootSignature->Finalize();
}
