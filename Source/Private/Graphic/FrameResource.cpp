#include "FrameResource.h"
#include "cassert"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount)
{
    assert(device);
    if (device)
    {
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())) >> Kds::App::Check;
    }
    PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
    ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
}

FrameResource::~FrameResource() {}