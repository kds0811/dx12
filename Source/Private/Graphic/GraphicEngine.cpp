#include "GraphicEngine.h"
#include "Device.h"
#include "DescriptorManager.h"

GraphicEngine::GraphicEngine() 
{
    mDevice = std::make_unique<Device>();
    mDescriptorManager = std::make_unique<DescriptorManager>();
}

GraphicEngine::~GraphicEngine() = default;

void GraphicEngine::Initialize()
{
    mDevice->Initialize();
    mDescriptorManager->Initialize(mDevice->GetDevice());
}
