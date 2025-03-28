#include "FrameResource.h"
#include "cassert"
#include "Logger.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount)
{
    assert(device);

    if (device)
    {
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())) >> Kds::App::Check;
    }
    assert(CmdListAlloc);

    PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);

    MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);

    ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);

    assert(PassCB);
    assert(MaterialCB);
    assert(ObjectCB);

    LOG_MESSAGE("Frame Resource has been created");
}

FrameResource::~FrameResource() {}