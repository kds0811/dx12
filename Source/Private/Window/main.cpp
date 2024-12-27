#include "Window/WindowDK.h"
#include "Helper.h"


 
//// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
//#include <wrl.h>
//using namespace Microsoft::WRL;
//
//#include <algorithm>
//#include <cassert>
//#include <chrono>
//
//#include <d3d12.h>
//#include <dxgi1_6.h>
//#include <d3dcompiler.h>
//#include <DirectXMath.h>
//#include <directx/d3dx12.h>
//
//const uint8_t g_NumFrames = 3;
//bool g_UseWarp = false;
//
//size_t g_ClientWidth = 1600;
//size_t g_ClientHeight = 1000;
//
//bool g_IsInitialized = false;
//
//
//// Window rectangle (used to toggle fullscreen state).
//RECT g_WindowRect;
//
//// DirectX 12 Objects
//ComPtr<ID3D12Device2> g_Device;
//ComPtr<ID3D12CommandQueue> g_CommandQueue;
//ComPtr<IDXGISwapChain4> g_SwapChain;
//ComPtr<ID3D12Resource> g_BackBuffers[g_NumFrames];
//ComPtr<ID3D12GraphicsCommandList> g_CommandList;
//ComPtr<ID3D12CommandAllocator> g_CommandAllocators[g_NumFrames];
//ComPtr<ID3D12DescriptorHeap> g_RTVDescriptorHeap;
//UINT g_RTVDescriptorSize;
//UINT g_CurrentBackBufferIndex;
//
//// Synchronization objects
//ComPtr<ID3D12Fence> g_Fence;
//uint64_t g_FenceValue = 0;
//uint64_t g_FrameFenceValues[g_NumFrames] = {};
//HANDLE g_FenceEvent;
//
//// By default, enable V-Sync.
//// Can be toggled with the V key.
//bool g_VSync = true;
//bool g_TearingSupported = false;
//// By default, use windowed mode.
//// Can be toggled with the Alt+Enter or F11
//bool g_Fullscreen = false;
//
//
//
//
//
//void EnableDebugLayer()
//{
//#if defined(_DEBUG)
//    ComPtr<ID3D12Debug> debugInterface;
//    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
//    debugInterface->EnableDebugLayer();
//#endif
//}

//
//void ParseCommandLineArguments()
//{
//    int argc;
//    wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
//
//    for (size_t i = 0; i < argc; ++i)
//    {
//        if (::wcscmp(argv[i], L"-w") == 0 || ::wcscmp(argv[i], L"--width") == 0)
//        {
//            g_ClientWidth = ::wcstol(argv[++i], nullptr, 10);
//        }
//        if (::wcscmp(argv[i], L"-h") == 0 || ::wcscmp(argv[i], L"--height") == 0)
//        {
//            g_ClientHeight = ::wcstol(argv[++i], nullptr, 10);
//        }
//        if (::wcscmp(argv[i], L"-warp") == 0 || ::wcscmp(argv[i], L"--warp") == 0)
//        {
//            g_UseWarp = true;
//        }
//    }
//
//    // Free memory allocated by CommandLineToArgvW
//    ::LocalFree(argv);
//}


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
