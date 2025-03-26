#pragma once
#include "GraphicCommonHeaders.h"

class Device
{
    Microsoft::WRL::ComPtr<IDXGIFactory7> mFactory = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Device8> mDevice = nullptr;

public:
    Device() = default;
    ~Device() = default;
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(const Device&& rhs) = delete;
    Device& operator=(const Device&& rhs) = delete;

    void Initialize();
    [[nodiscard]] inline ID3D12Device* GetDevice() const noexcept { return mDevice.Get(); }


};