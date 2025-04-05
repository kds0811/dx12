#include "GraphicEngine.h"
#include "Device.h"
#include "CommandManager.h"
#include "DescriptorHeapManager.h"
#include "Renderer.h"


GraphicEngine::GraphicEngine(HWND windowHandle)
{
    mDevice = std::make_unique<Device>();
    mCommandManager = std::make_unique<CommandManager>(mDevice->GetDevice());
    mDescriptorHeapManager = std::make_unique<DescriptorHeapManager>(mDevice->GetDevice());
    mRenderer = std::make_unique<Renderer>(windowHandle);

}

GraphicEngine::~GraphicEngine() = default;

void GraphicEngine::Initialize()
{
}
