#include "Window/WindowDK.h"

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <directx/d3dx12.h>
#include "GraphicError.h"

using namespace Microsoft::WRL;
using namespace kds::app;




int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    Window wnd(800, 600);


    DirectX::XMVECTOR vec = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

    try
    {
        ComPtr<ID3D12Device2> Device;
        D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&Device)) >> Chk;

        while (true)
        {
            
            if (const auto ecode = Window::PrecessMessages())
            {
                return *ecode;
            }
        }
    }
    catch (std::exception e)
    {
        MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR | MB_SETFOREGROUND);
    }
    catch (...)
    {

    }

    return 0;
}
