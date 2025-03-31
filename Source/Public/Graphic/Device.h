#pragma once
#include "GraphicCommonHeaders.h"

class Device
{
    static inline Microsoft::WRL::ComPtr<IDXGIFactory7> mFactory = nullptr;
    static inline Microsoft::WRL::ComPtr<ID3D12Device8> mDevice = nullptr;

public:
    Device();
    ~Device() = default;
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(const Device&& rhs) = delete;
    Device& operator=(const Device&& rhs) = delete;

    [[nodiscard]] static inline ID3D12Device* GetDevice() { return mDevice.Get(); }
    [[nodiscard]] static inline IDXGIFactory* GetFactory() { return mFactory.Get(); }
    [[nodiscard]] static inline ID3D12Device8* GetDevice8()  { return mDevice.Get(); }
    [[nodiscard]] static inline IDXGIFactory7* GetFactory7()  { return mFactory.Get(); }


private:
    void Initialize();
};