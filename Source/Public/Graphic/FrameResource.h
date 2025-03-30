#pragma once
#include "D3D12Utils.h"
#include "UploadBuffer.h"
#include "GraphicCommonStructs.h"

// Stores the resources needed for the CPU to build the command lists
// for a frame.
class FrameResource
{
private:
    std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
    std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
    std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

public:
    FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount);
    FrameResource(const FrameResource& rhs) = delete;
    FrameResource& operator=(const FrameResource& rhs) = delete;
    FrameResource(const FrameResource&& rhs) = delete;
    FrameResource& operator=(const FrameResource&& rhs) = delete;
    ~FrameResource();

    [[nodiscard]] inline UploadBuffer<PassConstants>* GetPassConstants() noexcept { return PassCB.get(); }
    [[nodiscard]] inline UploadBuffer<MaterialConstants>* GetMaterialConstants() noexcept { return MaterialCB.get(); }
    [[nodiscard]] inline UploadBuffer<ObjectConstants>* GetObjectConstants() noexcept { return ObjectCB.get(); }

private:
    void Initialize(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount);
};