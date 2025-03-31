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

    [[nodiscard]] static inline ID3D12Device* GetDevice() noexcept { return mDevice.Get(); }
    [[nodiscard]] static inline IDXGIFactory* GetFactory() noexcept { return mFactory.Get(); }
    [[nodiscard]] static inline ID3D12Device8* GetDevice8() noexcept { return mDevice.Get(); }
    [[nodiscard]] static inline IDXGIFactory7* GetFactory7() noexcept { return mFactory.Get(); }


private:
    void Initialize();
};