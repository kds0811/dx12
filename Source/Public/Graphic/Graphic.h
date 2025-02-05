#pragma once
#include "D3D12Utils.h"
#include <memory>
#include "UploadBuffer.h"
#include "MathHelper.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include "PrimitiveSceneObject.h"
#include "WavesSceneObject.h"

class GameTimerW;

using namespace Microsoft::WRL;

class Graphic
{
private:
    // Main Fields
    ComPtr<ID3D12Debug3> mDebugController;
    ComPtr<IDXGIFactory7> mFactory;
    ComPtr<ID3D12Device8> mDevice;

    // Command Stuff
    ComPtr<ID3D12Fence1> mFence;
    UINT64 mCurrentFenceValue = 0;
    ComPtr<ID3D12CommandQueue> mCommandQueue;
    ComPtr<ID3D12CommandAllocator> mCommandAlloc;
    ComPtr<ID3D12GraphicsCommandList6> mCommandList;

    // SwapChain and Backbuffer Fields
    static constexpr UINT mSwapChainBufferCount = 2;
    ComPtr<IDXGISwapChain4> mSwapChain;
    ComPtr<ID3D12Resource> mSwapChainBuffer[mSwapChainBufferCount];
    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    int mCurrBackBuffer = 0;

    // Depth Stencil Fields
    ComPtr<ID3D12Resource> mDepthStencilBuffer;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // Decriptor Heaps
    ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    ComPtr<ID3D12DescriptorHeap> mDsvHeap;

    // Decriptor Sizes
    UINT mRtvDescriptorSize = 0u;
    UINT mDsvDescriptorSize = 0u;
    UINT mCbvSrvUavDescriptorSize = 0u;

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
    std::vector<std::unique_ptr<FrameResource>> mFrameResources;
    FrameResource* mCurrFrameResource = nullptr;
    int mCurrFrameResourceIndex = 0;

    ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
    ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

    std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    PassConstants mMainPassCB;

    UINT mPassCbvOffset = 0;

    bool bIsWireframe = false;

    DirectX::XMFLOAT3 mEyePos = {0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

    size_t mSceneObjectCount = 0;
    size_t mWavesVerticesCount = 0;
    size_t mMaterialCount = 0;

public:
    Graphic(UINT Width, UINT Height, HWND hwnd);
    ~Graphic();

    float GetAspectRatio() const;
    void OnResize(UINT nWidth, UINT nHeight);
    void Draw(const std::vector<std::unique_ptr<BaseSceneObject>>& sceneObjects);

    void Update(DirectX::FXMMATRIX ViewMat, DirectX::XMFLOAT3 CameraPos, const GameTimerW* gt,
        const std::vector<std::unique_ptr<BaseSceneObject>>& sceneObjects, WavesSceneObject* waveObject,
        std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials);

    void SetWireframe(bool state);
    ID3D12Device8* GetDevice() { return mDevice.Get(); }
    ID3D12GraphicsCommandList6* GetCommandList() { return mCommandList.Get(); }
    ID3D12CommandQueue* GetCommandQueue() { return mCommandQueue.Get(); }

    void InitResources(size_t sceneObjectCount, size_t wavesVertCount, size_t materialsCount,
        std::unordered_map<EMaterialType, std::unique_ptr<Texture>>& textures);
    

private:
    void InitPipeline();
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const noexcept;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const noexcept;
    ID3D12Resource* CurrentBackBuffer() const noexcept;
    void FlushCommandQueue();

    void UpdateMainPassCB(const GameTimerW* gt);
    void UpdateObjectCBs(const std::vector<std::unique_ptr<BaseSceneObject>>& sceneObjects);
    void UpdateMaterialCBs(std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials);

    void BuildDescriptorHeaps(std::unordered_map<EMaterialType, std::unique_ptr<Texture>>& textures);
    void BuildConstantBufferViews();
    void BuildRootSignature();
    void BuildShadersAndInputLayout();
    
    void BuildPSOs();
    void BuildFrameResources();
    void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<std::unique_ptr<BaseSceneObject>>& sceneObjects);

    void UpdateWavesMesh(const GameTimerW* gt, WavesSceneObject* waveObject);
};
