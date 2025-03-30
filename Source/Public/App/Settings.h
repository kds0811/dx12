#pragma once
#include "GraphicCommonHeaders.h"


struct Settings
{
    static constexpr size_t mNumFrameResources = 3;
    static constexpr DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    static inline UINT mWidth = 1600;
    static inline UINT mHeight = 800;

};