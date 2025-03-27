#pragma once
#include "D3D12Utils.h"
#include <memory>
#include "UploadBuffer.h"
#include "MathHelper.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include "PrimitiveSceneObject.h"
#include "WavesSceneObject.h"
#include "Scene.h"
#include "BlurFilter.h"
#include "SobelFilter.h"
#include "RenderTarget.h"
#include "GpuWaves.h"
#include "DirectXCollision.h"
#include "Material.h"

class GameTimerW;

class Graphic
{
private:
    // Main Fields


   

    // SwapChain and Backbuffer Fields
    static constexpr UINT mSwapChainBufferCount = 2;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[mSwapChainBufferCount];
    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    int mCurrBackBuffer = 0;

    // Depth Stencil Fields
    Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // Decriptor Heaps
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;



    // Viewport and window fields
    D3D12_VIEWPORT mScreenViewport;
    UINT mClientWidth = 0u;
    UINT mClientHeight = 0u;
    D3D12_RECT mScissorRect;
    HWND mWindowHandle = nullptr;

    // Driver Type
    D3D_DRIVER_TYPE mD3dDriverType = D3D_DRIVER_TYPE_HARDWARE;

private:
    // Frame Resources


   Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mPostProcessRootSignature = nullptr;
   Microsoft::WRL::ComPtr<ID3D12RootSignature> mPostBilateralRootSignature = nullptr;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvSrvUavDescriptorHeap = nullptr;

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mStdInputLayout;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mTreeSpriteInputLayout;

    PassConstants mMainPassCB;
    PassConstants mReflectedPassCB;

    UINT mPassCbvOffset = 0;

    bool bIsWireframe = false;
    bool bIsSobelEnabled = false;
    bool bIsBlurEnabled = false;

    DirectX::XMFLOAT3 mEyePos = {0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

    size_t mSceneObjectCount = 0;
    size_t mWavesVerticesCount = 0;
    size_t mMaterialCount = 0;

    std::unique_ptr<BlurFilter> mBilateralFilter;

    std::unique_ptr<RenderTarget> mOffscreenRT = nullptr;

    std::unique_ptr<SobelFilter> mSobelFilter = nullptr;


public:
    // Lights
    DirectX::XMFLOAT4 mAmbientLight = {0.2f, 0.2f, 0.25f, 1.0f};
    DirectX::XMFLOAT3 mLightsDirection = {0.57735f, -0.57735f, -0.57735f};
    DirectX::XMFLOAT3 mLightsStrength = {0.5f, 0.5f, 0.5f};

    // Blur settings
    int mBlurCount = 1;
    float mBlurSigma = 1.0f;
    float mBlurWeights = 1.5f;

public:
    Graphic(UINT Width, UINT Height, HWND hwnd);
    ~Graphic();

    float GetAspectRatio() const;
    void OnResize(UINT nWidth, UINT nHeight);

    // Draw
    void StartDrawFrame(const SortedSceneObjects& sortedSceneObjects);
    void EndDrawFrame();

    void Update(DirectX::FXMMATRIX ViewMat, DirectX::XMFLOAT3 CameraPos, const GameTimerW* gt, const std::vector<std::unique_ptr<BaseSceneObject>>& sceneObjects,
        WavesSceneObject* waveObject, std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials);

    void SetWireframe(bool state);
    void SetSobel(bool state);
    void SetBlur(bool state);

    ID3D12Device8* GetDevice() { return mDevice.Get(); }
    ID3D12GraphicsCommandList6* GetCommandList() { return mCommandList.Get(); }
    ID3D12CommandQueue* GetCommandQueue() { return mCommandQueue.Get(); }

    void InitResources(size_t sceneObjectCount, size_t wavesVertCount, size_t materialsCount, std::unordered_map<EMaterialType, std::unique_ptr<Texture>>& textures);

    DXGI_FORMAT GetBackBufferFormat() const { return mBackBufferFormat; }
    DXGI_FORMAT GetDepthStencilFormat() const { return mDepthStencilFormat; }
    ID3D12DescriptorHeap* GetRtvDescriptorHeap() { return mRtvHeap.Get(); }
    ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return mCbvSrvUavDescriptorHeap.Get(); }

    PassConstants& GetMainPassCB() { return mMainPassCB; }

private:
    void InitPipeline();
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const noexcept;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const noexcept;
    ID3D12Resource* CurrentBackBuffer() const noexcept;
    void FlushCommandQueue();

    void UpdateMainPassCB(const GameTimerW* gt);
    void UpdateReflectedPassCB();
    void UpdateObjectCBs(const std::vector<std::unique_ptr<BaseSceneObject>>& sceneObjects);
    void UpdateMaterialCBs(std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials);

    void BuildDescriptorHeaps(std::unordered_map<EMaterialType, std::unique_ptr<Texture>>& textures);
    void BuildRootSignature();
    void BuildPostProcessRootSignature();
    void BuildBilateralRootSignature();

    void BuildShadersAndInputLayout();

    void BuildPSOs();
    void BuildFrameResources();

    void DrawRenderItems(const std::vector<BaseSceneObject*>& sceneObjects, bool isReflectedObjects);
    void DrawShadows(const std::vector<BaseSceneObject*>& sceneObjects);

    void UpdateWavesMesh(const GameTimerW* gt, WavesSceneObject* waveObject);
    void DrawFullscreenQuad(ID3D12GraphicsCommandList* cmdList);
};
