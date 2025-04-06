#pragma once
#include "GraphicCommonHeaders.h"
#include "GpuResource.h"

class DepthStencil : public GpuResource
{
    UINT mWidth = 0;
    UINT mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DescriptorHandle mDsvDescriptorHandle{};

public:
    DepthStencil() = default;
    DepthStencil(const DepthStencil& rhs) = delete;
    DepthStencil& operator=(const DepthStencil& rhs) = delete;
    ~DepthStencil();

    bool Initialize(std::wstring name, UINT width, UINT height, DXGI_FORMAT format);
    bool InitializeFromExistingResource(ID3D12Resource* existingResource, std::wstring name, DXGI_FORMAT format);

    [[nodiscard]] inline DescriptorHandle& GetDsvDescriptorHandle() noexcept { return mDsvDescriptorHandle; }
    [[nodiscard]] bool IsInitialized() const noexcept { return mResource && !mDsvDescriptorHandle.IsNull(); }
    void OnResize(UINT newWidth, UINT newHeight);

private:
    [[nodiscard]] bool BuildResource();
    [[nodiscard]] bool BuildDescriptors();

    void Destroy();
    void DestroyDescriptorHandles();
};