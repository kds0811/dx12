#pragma once
#include <memory>

class Device;
class CommandManager;
class DescriptorHeapManager;
class SwapChain;


class GraphicEngine
{
    std::unique_ptr<Device> mDevice;
    std::unique_ptr<CommandManager> mCommandManager;
    std::unique_ptr<DescriptorHeapManager> mDescriptorHeapManager;
    std::unique_ptr<SwapChain> mSwapChain;

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