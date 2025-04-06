#pragma once
#include "GraphicCommonHeaders.h"
#include "GpuResource.h"

class RenderTarget : public GpuResource
{
    UINT mWidth = 0;
    UINT mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    D3D12_CLEAR_VALUE mClearValue{};
    D3D12_RESOURCE_DIMENSION mResourceDimension{};
    DescriptorHandle mSrvDescriptorHandle{};
    DescriptorHandle mRtvDescriptorHandle{};


public:
    RenderTarget() = default;
    bool Initialize(std::wstring name, UINT width, UINT height, DXGI_FORMAT format, D3D12_CLEAR_VALUE clearValue, D3D12_RESOURCE_DIMENSION resourceDimension);
    bool InitializeAsBackBuffer(ID3D12Resource* existingResource, std::wstring name, DXGI_FORMAT format);


    RenderTarget(const RenderTarget& rhs) = delete;
    RenderTarget& operator=(const RenderTarget& rhs) = delete;
    ~RenderTarget();

    [[nodiscard]] inline DescriptorHandle& GetSrvDescriptorHandle() noexcept { return mSrvDescriptorHandle; }
    [[nodiscard]] inline DescriptorHandle& GetRtvDescriptorHandle() noexcept { return mRtvDescriptorHandle; }
    [[nodiscard]] bool IsInitialized() const noexcept { return mResource && !mSrvDescriptorHandle.IsNull() && !mRtvDescriptorHandle.IsNull(); }
    void OnResize(UINT newWidth, UINT newHeight);

private:
    [[nodiscard]] bool BuildResource();
    [[nodiscard]] bool BuildDescriptors();

    void Destroy();
    void DestroyDescriptorHandles();

};