#include "GraphicEngine.h"
#include "Device.h"
#include "CommandManager.h"
#include "Renderer.h"
#include "ResourceManager.h"


GraphicEngine::GraphicEngine(HWND windowHandle)
{
    mDevice = std::make_unique<Device>();
    mCommandManager = std::make_unique<CommandManager>(mDevice->GetDevice());
    mResourceManager = std::make_unique<ResourceManager>();
    mRenderer = std::make_unique<Renderer>(windowHandle);

}

GraphicEngine::~GraphicEngine() = default;

void GraphicEngine::Initialize()
{
}
