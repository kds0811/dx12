#include "Window/WindowDK.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <directx/d3dx12.h>




int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    Window wnd(800, 600);
    while (true)
    {
        if (const auto ecode = Window::PrecessMessages())
        {
            return *ecode;
        }
    }

    return 0;
}
