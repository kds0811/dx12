#pragma once
#include "DescriptorHeapAllocator.h"

class Graphic;

static DescriptorHeapAllocator g_pd3dSrvDescHeapAlloc;

class ImguiWrapper
{
    Graphic* pGfx = nullptr;

public:
    ImguiWrapper();
    ~ImguiWrapper();

    void InitImgui(Graphic* gfx, HWND hwnd);
    void StartImguiFrame();
    void EndImguiFrame();

};