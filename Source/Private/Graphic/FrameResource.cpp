#include "FrameResource.h"
#include "UploadBuffer.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount)
{
    Initialize(device,  passCount, objectCount, materialCount);
}

FrameResource::~FrameResource() = default;

void FrameResource::Initialize(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount)
{
    assert(device);

    PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);

    MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);

    ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);

    assert(PassCB);
    assert(MaterialCB);
    assert(ObjectCB);

    LOG_MESSAGE("Frame Resource has been created");
}