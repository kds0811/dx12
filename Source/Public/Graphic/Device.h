#pragma once
#include "GraphicCommonHeaders.h"

class Device
{
    Microsoft::WRL::ComPtr<IDXGIFactory7> mFactory = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Device8> mDevice = nullptr;

public:
    Device() = default;
    bool Initialize();
    [[nodiscard]] inline ID3D12Device* GetDevice() const noexcept { return mDevice.Get(); }


};