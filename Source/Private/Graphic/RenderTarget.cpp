//***************************************************************************************
// RenderTarget.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "RenderTarget.h"

RenderTarget::RenderTarget(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
    mDevice = device;

    mWidth = width;
    mHeight = height;
    mFormat = format;

    mTexture = std::make_unique<GpuResource>(mDevice, mWidth, mHeight, L"OffscreenTex", D3D12_RESOURCE_STATE_GENERIC_READ, mFormat, DirectX::XMFLOAT4{0.7f, 0.7f, 0.7f, 1.0f});
}

ID3D12Resource* RenderTarget::Resource()
{
    return mTexture->GetResource();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE RenderTarget::Srv()
{
    return mhGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTarget::Rtv()
{
    return mhCpuRtv;
}

void RenderTarget::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv)
{
    // Save references to the descriptors.
    mhCpuSrv = hCpuSrv;
    mhGpuSrv = hGpuSrv;
    mhCpuRtv = hCpuRtv;

    BuildDescriptors();
}

void RenderTarget::OnResize(UINT newWidth, UINT newHeight)
{
    if ((mWidth != newWidth) || (mHeight != newHeight))
    {
        mWidth = newWidth;
        mHeight = newHeight;

        mTexture->OnResize(mWidth, mHeight);

        // New resource, so we need new descriptors to that resource.
        BuildDescriptors();
    }
}

void RenderTarget::BuildDescriptors()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = mFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    mDevice->CreateShaderResourceView(mTexture->GetResource(), &srvDesc, mhCpuSrv);

    mDevice->CreateRenderTargetView(mTexture->GetResource(), nullptr, mhCpuRtv);
}
