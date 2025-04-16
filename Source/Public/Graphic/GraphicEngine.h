#pragma once
#include <memory>

class Device;
class CommandManager;
class ResourceManager;
class Renderer;

class GraphicEngine
{
    std::unique_ptr<Device> mDevice = nullptr;
    std::unique_ptr<CommandManager> mCommandManager = nullptr;
    std::unique_ptr<ResourceManager> mResourceManager = nullptr;
    std::unique_ptr<Renderer> mRenderer = nullptr;

public:
    GraphicEngine(HWND windowHandle);
    ~GraphicEngine();
    GraphicEngine(const GraphicEngine&) = delete;
    GraphicEngine& operator=(const GraphicEngine&) = delete;
    GraphicEngine(const GraphicEngine&& rhs) = delete;
    GraphicEngine& operator=(const GraphicEngine&& rhs) = delete;

private:
    void Initialize();
};