//***************************************************************************************
// SobelFilter.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "SobelFilter.h"

SobelFilter::SobelFilter(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
    mDevice = device;

    mWidth = width;
    mHeight = height;
    mFormat = format;

    mOutput =
        std::make_unique<GpuResource>(mDevice, mWidth, mHeight, L"SobelFilterResource", D3D12_RESOURCE_STATE_GENERIC_READ, mFormat, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
}

CD3DX12_GPU_DESCRIPTOR_HANDLE SobelFilter::OutputSrv()
{
    return mhGpuSrv;
}

UINT SobelFilter::DescriptorCount() const
{
    return 2;
}

void SobelFilter::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor, UINT descriptorSize)
{
    // Save references to the descriptors.
    mhCpuSrv = hCpuDescriptor;
    mhCpuUav = hCpuDescriptor.Offset(1, descriptorSize);
    mhGpuSrv = hGpuDescriptor;
    mhGpuUav = hGpuDescriptor.Offset(1, descriptorSize);

    BuildDescriptors();
}

void SobelFilter::OnResize(UINT newWidth, UINT newHeight)
{
    if ((mWidth != newWidth) || (mHeight != newHeight))
    {
        mWidth = newWidth;
        mHeight = newHeight;

        mOutput->OnResize(mWidth, mHeight);

        // New resource, so we need new descriptors to that resource.
        BuildDescriptors();
    }
}

void SobelFilter::Execute(ID3D12GraphicsCommandList* cmdList, ID3D12RootSignature* rootSig, ID3D12PipelineState* pso, CD3DX12_GPU_DESCRIPTOR_HANDLE input)
{
    cmdList->SetComputeRootSignature(rootSig);
    cmdList->SetPipelineState(pso);

    cmdList->SetComputeRootDescriptorTable(0, input);
    cmdList->SetComputeRootDescriptorTable(2, mhGpuUav);

    mOutput->ChangeState(cmdList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    // How many groups do we need to dispatch to cover image, where each
    // group covers 16x16 pixels.
    UINT numGroupsX = (UINT)ceilf(mWidth / 16.0f);
    UINT numGroupsY = (UINT)ceilf(mHeight / 16.0f);
    cmdList->Dispatch(numGroupsX, numGroupsY, 1);

    mOutput->ChangeState(cmdList, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void SobelFilter::BuildDescriptors()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = mFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

    uavDesc.Format = mFormat;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;

    mDevice->CreateShaderResourceView(mOutput->GetResource(), &srvDesc, mhCpuSrv);
    mDevice->CreateUnorderedAccessView(mOutput->GetResource(), nullptr, &uavDesc, mhCpuUav);
}
