#pragma once
#include "D3D12Utils.h"
#include "GpuResource.h"
#include "memory"


class RenderTarget
{
public:
    RenderTarget(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format);

    RenderTarget(const RenderTarget& rhs) = delete;
    RenderTarget& operator=(const RenderTarget& rhs) = delete;
    ~RenderTarget() = default;

    GpuResource* GetGpuResource();
    ID3D12Resource* Resource();
    CD3DX12_GPU_DESCRIPTOR_HANDLE Srv();
    CD3DX12_CPU_DESCRIPTOR_HANDLE Rtv();

    void BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv);

    void OnResize(UINT newWidth, UINT newHeight);

private:
    void BuildDescriptors();

private:
    ID3D12Device* mDevice = nullptr;

    UINT mWidth = 0;
    UINT mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuRtv;

    //Microsoft::WRL::ComPtr<ID3D12Resource> mOffscreenTex = nullptr;
    std::unique_ptr<GpuResource> mTexture;
};
