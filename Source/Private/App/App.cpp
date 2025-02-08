#include "App.h"
#include <string>
#include <thread>
#include <format>
#include <DirectXMath.h>
#include "Vector.h"
#include <chrono>
#include <cassert>
#include "WavesSceneObject.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "DescriptorHeapAllocator.h"

static DescriptorHeapAllocator g_pd3dSrvDescHeapAlloc;



App::App()
{
    mWnd = std::make_unique<Window>(mWidth, mHeight, this);
    mGfx = std::make_unique<Graphic>(mWidth, mHeight, mWnd->GetHwnd());
    mTimer = std::make_unique<GameTimerW>();
    mCamera = std::make_unique<Camera>(this);
    mCameraController = std::make_unique<CameraController>(mWnd.get(), mCamera.get(), mTimer.get());
    mResourceManager = std::make_unique<ResourceManager>(mGfx->GetDevice(), mGfx->GetCommandQueue());
    mScene = std::make_unique<Scene>(mTimer.get(), mResourceManager.get());

    assert(mWnd);
    assert(mGfx);
    assert(mTimer);
    assert(mCamera);
    assert(mCameraController);
    assert(mScene);
    assert(mResourceManager);

    if (mScene)
    {
        mScene->InitScene();
    }
    if (mGfx)
    {
        assert(mScene);
        assert(mScene->GetWavesPtr());
        mGfx->InitResources(mScene->GetSceneRenderItems().size(), mScene->GetWavesPtr()->GetVertexCount(),
            mResourceManager->GetMaterials().size(), mResourceManager->GetTextures());
    }

    g_pd3dSrvDescHeapAlloc.Create(mGfx->GetDevice(), mGfx->GetSrvDescriptorHeap());
    InitImgui();
}

App::~App()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

std::optional<int> App::Go()
{
    mTimer->Reset();

    while (true)
    {
        mTimer->Tick();
        CalculateFrameStats();
        Update();
        Draw();
        if (const auto ecode = Window::PrecessMessages())
        {
            return *ecode;
        }
    }
    return std::nullopt;
}

void App::OnResize(UINT width, UINT height)
{
    mWidth = width;
    mHeight = height;
    mGfx->OnResize(width, height);
}

void App::OnStop()
{
    if (!bAppPaused)
    {
        bAppPaused = true;
        mTimer->Stop();
    }
}

void App::OnStart()
{
    if (bAppPaused)
    {
        bAppPaused = false;
        mTimer->Start();
    }
}

void App::Update()
{
    mCameraController->UpdateInput();
    mScene->Update();
    mGfx->Update(mCamera->GetViewMatrix(), mCamera->GetCameraPos(), mTimer.get(), mScene->GetSceneObjects(), mScene->GetWavesPtr(),
        mResourceManager->GetMaterials());
}

void App::Draw()
{
    StartImguiFrame();
    mGfx->StartDrawFrame(mScene->GetSceneObjects());
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mGfx->GetCommandList());
    mGfx->EndDrawFrame();
}

void App::SetWireframe(bool wireframeIsEnabled)
{
    mGfx->SetWireframe(wireframeIsEnabled);
}

void App::CalculateFrameStats()
{
    static int frameCnt = 0;
    static double timeElapsed = 0.0f;

    frameCnt++;

    if ((mTimer->GetTotalTime() - timeElapsed) >= 1.0)
    {
        double fps = static_cast<double>(frameCnt);
        double mspf = 1000.0 / fps;

        std::string windowText =
            std::format("{} FPS : {:.2f} MSPF {:.2f} TOTAL TIME : {:.2f} ", mWnd->GetTitle(), fps, mspf, mTimer->GetTotalTime());

        SetWindowText(mWnd->GetHwnd(), windowText.c_str());

        frameCnt = 0;
        timeElapsed += 1.0;
    }
}

void App::InitImgui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(mWnd->GetHwnd());

    // Setup Platform/Renderer backends
    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = mGfx->GetDevice();
    init_info.CommandQueue = mGfx->GetCommandQueue();
    init_info.NumFramesInFlight = gNumFrameResources;
    init_info.RTVFormat = mGfx->GetBackBufferFormat();
    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

    // Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
    // The example_win32_directx12/main.cpp application include a simple free-list based allocator.
    init_info.SrvDescriptorHeap = mGfx->GetSrvDescriptorHeap();

    init_info.SrvDescriptorAllocFn =
        [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
    { return g_pd3dSrvDescHeapAlloc.Alloc(out_cpu_handle, out_gpu_handle); };

    init_info.SrvDescriptorFreeFn =
        [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
    { return g_pd3dSrvDescHeapAlloc.Free(cpu_handle, gpu_handle); };

    ImGui_ImplDX12_Init(&init_info);
}

void App::StartImguiFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    //ImGui::ShowDemoWindow();

  

    if (ImGui::Begin("Fog Settings", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::SliderFloat("Fog Start", &mGfx->GetMainPassCB().gFogStart, 0.0f, 100.0f);
        ImGui::SliderFloat("Fog Range", &mGfx->GetMainPassCB().gFogRange, 100.0f, 500.0f);
        ImGui::ColorEdit4("Fog Color", (float*)&mGfx->GetMainPassCB().FogColor); 
        ImGui::End();  
    }
}
