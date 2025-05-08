#include "PsoManager.h"
#include "Pso.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "RootSignatureManager.h"
#include "InputLayoutManager.h"
#include "Settings.h"

PsoManager::PsoManager() 
{
    mRootSignatureManager = std::make_unique<RootSignatureManager>();
    mShaderManager = std::make_unique<ShaderManager>();
    mInputLauoutManager = std::make_unique<InputLayoutManager>();
    BuildPso();
}

PsoManager::~PsoManager() = default;

void PsoManager::BuildPso() 
{
    BuildGraphicPsos();
    BuildComputePsos();
}

void PsoManager::BuildGraphicPsos() 
{
    std::wstring nameOpaquePso = L"opaque";
    std::unique_ptr<GraphicPso> basePso = std::make_unique<GraphicPso>(mRootSignatureManager->GetRootSignature(L"base"));
    basePso->SetVertexShader(mShaderManager->GetVertexShader("standard")->GetByteCode());
    basePso->SetPixelShader(mShaderManager->GetPixelShader("opaque")->GetByteCode());
    basePso->SetRasterizerState(CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT));
    basePso->SetFillMode(D3D12_FILL_MODE_SOLID);
    basePso->SetBlendState(CD3DX12_BLEND_DESC(D3D12_DEFAULT));
    basePso->SetDepthStencilState(CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT));
    basePso->SetSampleMask(UINT_MAX);
    basePso->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    basePso->SetNumberRenderTargets(1u);
    basePso->SetRenderTargetFormat(Settings::mBackBufferFormat, Settings::mDepthStencilFormat);
    basePso->Finalize(nameOpaquePso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameOpaquePso] = std::move(basePso);

    std::wstring nameWireframePso = L"wireframe";
    std::unique_ptr<GraphicPso> wireframePso = std::make_unique<GraphicPso>(*mGraphicPsos.at(nameOpaquePso));
    wireframePso->SetFillMode(D3D12_FILL_MODE_WIREFRAME);
    wireframePso->Finalize(nameWireframePso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameWireframePso] = std::move(wireframePso);
}

void PsoManager::BuildComputePsos() 
{

}

