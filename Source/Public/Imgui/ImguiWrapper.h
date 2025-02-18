#pragma once
#include "DescriptorHeapAllocator.h"

class Graphic;
class Camera;

static DescriptorHeapAllocator g_pd3dSrvDescHeapAlloc;

class ImguiWrapper
{
    Graphic* pGfx = nullptr;
    Camera* pCamera = nullptr;

public:
    ImguiWrapper();
    ~ImguiWrapper();

    void InitImgui(Graphic* gfx, HWND hwnd, size_t numTextures, Camera* cam);
    void StartImguiFrame();
    void EndImguiFrame();

};