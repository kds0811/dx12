#include "ImguiWrapper.h"
#include <Windows.h>
#include "Graphic.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "GraphicError.h"
#include "Camera.h"

ImguiWrapper::ImguiWrapper() {}

ImguiWrapper::~ImguiWrapper()
{
    g_pd3dSrvDescHeapAlloc.Destroy();
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImguiWrapper::InitImgui(Graphic* gfx, HWND hwnd, size_t numTextures, Camera* cam)
{
    pGfx = gfx;
    pCamera = cam;
    g_pd3dSrvDescHeapAlloc.Create(pGfx->GetDevice(), pGfx->GetSrvDescriptorHeap(), numTextures);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);

    // Setup Platform/Renderer backends
    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = pGfx->GetDevice();
    init_info.CommandQueue = pGfx->GetCommandQueue();
    init_info.NumFramesInFlight = gNumFrameResources;
    init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

    // Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
    init_info.SrvDescriptorHeap = pGfx->GetSrvDescriptorHeap();

    init_info.SrvDescriptorAllocFn =
        [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
    { return g_pd3dSrvDescHeapAlloc.Alloc(out_cpu_handle, out_gpu_handle); };

    init_info.SrvDescriptorFreeFn =
        [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
    { return g_pd3dSrvDescHeapAlloc.Free(cpu_handle, gpu_handle); };

    ImGui_ImplDX12_Init(&init_info);
}

void ImguiWrapper::StartImguiFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    // ImGui::ShowDemoWindow();

    if (ImGui::Begin("Fog Settings", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::SliderFloat("Fog Start", &pGfx->GetMainPassCB().gFogStart, 0.0f, 100.0f);
        ImGui::SliderFloat("Fog Range", &pGfx->GetMainPassCB().gFogRange, 100.0f, 1000.0f);
        ImGui::ColorEdit4("Fog Color", (float*)&pGfx->GetMainPassCB().FogColor);
        ImGui::End();
    }

    if (ImGui::Begin("Light Settings", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::ColorEdit4("Ambient Light", (float*)&pGfx->mAmbientLight);
        ImGui::DragFloat3("Light Direction", &pGfx->mLightsDirection.x, 0.1f, -1.0f, 1.0f);
        ImGui::ColorEdit3("Light Color", (float*)&pGfx->mLightsStrength);
        ImGui::End();
    }

    if (ImGui::Begin("Camera Speed", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::SliderFloat("Camera Speed", &pCamera->GetSpeedCameraRef(), 1.0f, 500.0f);
        ImGui::SliderFloat("Camera Rotate", &pCamera->GetSpeedRotateCameraRef(), 1.0f, 100.0f);
        ImGui::End();
    }
}

void ImguiWrapper::EndImguiFrame()
{
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pGfx->GetCommandList());
}
