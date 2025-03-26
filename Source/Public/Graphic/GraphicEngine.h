#pragma once
#include <memory>

//forward declaration
class Device;
class DescriptorManager;

class GraphicEngine
{
    std::unique_ptr<Device> mDevice;
    std::unique_ptr<DescriptorManager> mDescriptorManager;

public:
    GraphicEngine();
    ~GraphicEngine();
    GraphicEngine(const GraphicEngine&) = delete;
    GraphicEngine& operator=(const GraphicEngine&) = delete;
    GraphicEngine(const GraphicEngine&& rhs) = delete;
    GraphicEngine& operator=(const GraphicEngine&& rhs) = delete;

private:
    void Initialize();
};