#include "Graphic.h"

Graphic::Graphic() 
{
    InitPipeline();
    
    

}

void Graphic::InitPipeline()
{
    EnableDebugLayer();
    CreateFactory();
    CreateDevice();
}

void Graphic::EnableDebugLayer()
{
#if defined(_DEBUG)
    D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugController)) >> Check;
    m_DebugController->EnableDebugLayer();
#endif
}

void Graphic::CreateFactory()
{
    UINT CreateFactoryFlags = 0;
#if defined(_DEBUG)
    CreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    CreateDXGIFactory2(CreateFactoryFlags, IID_PPV_ARGS(&m_Factory)) >> Check;
}

void Graphic::CreateDevice() 
{
    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&m_Device)) >> Check;
}
