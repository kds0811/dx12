#include "Device.h"
#include "Logger.h"

using namespace DirectX;
using namespace Kds::App;
using namespace Microsoft::WRL;

void Device::Initialize()
{
    // enable debug layer
#if defined(_DEBUG)
    ComPtr<ID3D12Debug> debugInterface;
    auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
    if (SUCCEEDED(hr))
    {
        debugInterface->EnableDebugLayer();
        LOG_MESSAGE("Enable Debug Layer");
    }
    else
    {
        LOG_ERROR("failure when creating a debug interface");
    }

    ComPtr<ID3D12Debug3> debugInterface3;
    hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface3));
    if (SUCCEEDED(hr))
    {
        debugInterface3->SetEnableGPUBasedValidation(true);
        debugInterface3->SetEnableSynchronizedCommandQueueValidation(true);
        LOG_MESSAGE("Enable GPUBasedValidation and SynchronizedCommandQueueValidation");
    }

    ComPtr<ID3D12Debug5> debugInterface5;
    hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface5));
    if (SUCCEEDED(hr))
    {
        debugInterface5->SetEnableAutoName(true);
        LOG_MESSAGE("Enable Auto Name D3D Objects on debug");
    }
#endif

    // create FACTORY
    UINT CreateFactoryFlags = 0;
#if defined(_DEBUG)
    CreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    CreateDXGIFactory2(CreateFactoryFlags, IID_PPV_ARGS(&mFactory)) >> Check;

    // Create DEVICE
    //
    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};

    // try create hardware device
    HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&mDevice));

    if (SUCCEEDED(hr))
    {
        LOG_MESSAGE("Device was created with D3D_FEATURE_LEVEL_12_2");
    }
    else
    {
        for (const auto& fLevel : featureLevels)
        {
            hr = D3D12CreateDevice(nullptr, fLevel, IID_PPV_ARGS(&mDevice));
            if (SUCCEEDED(hr))
            {
                LOG_MESSAGE("Device was created with D3D_FEATURE_LEVEL_12_1 or above");
                break;
            }
        }
        // if create hardware device is failed, try create WARP device
        if (FAILED(hr))
        {
            ComPtr<IDXGIAdapter> pWarpAdapter;
            if (mFactory)
            {
                mFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)) >> Check;
                hr = D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mDevice));

                if (SUCCEEDED(hr))
                {
                    LOG_MESSAGE("Device was created as WARP");
                }
                else
                {
                    LOG_MESSAGE("Device has not been created");
                }
            }
        }
    }

    mDevice->SetName(L"Device");

#if defined(_DEBUG)
    ComPtr<ID3D12InfoQueue> pInfoQueue = nullptr;
    if (mDevice)
    {
        mDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
        if (pInfoQueue)
        {
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        }
    }
#endif
}
