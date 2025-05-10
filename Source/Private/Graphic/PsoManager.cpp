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
}

void PsoManager::BuildGraphicPsos() 
{
    //
    // OPAQUE PSO
    //
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

    //
    // WIREFRAME PSO
    //
    std::wstring nameWireframePso = L"wireframe";
    std::unique_ptr<GraphicPso> wireframePso = std::make_unique<GraphicPso>(*mGraphicPsos.at(nameOpaquePso));
    wireframePso->SetFillMode(D3D12_FILL_MODE_WIREFRAME);
    wireframePso->Finalize(nameWireframePso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameWireframePso] = std::move(wireframePso);

    //
    // TRANSPARENT PSO
    //
    std::wstring nameTransparentPso = L"transparent";
    std::unique_ptr<GraphicPso> transparentPso = std::make_unique<GraphicPso>(*mGraphicPsos.at(nameOpaquePso));
    D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
    transparencyBlendDesc.BlendEnable = true;
    transparencyBlendDesc.LogicOpEnable = false;
    transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
    transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
    transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
    transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
    transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
    transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    transparentPso->SetRenderTargetBlendDesc(transparencyBlendDesc);
    transparentPso->Finalize(nameTransparentPso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameTransparentPso] = std::move(transparentPso);

    //
    // ALPHA TESTED PSO
    //
    std::wstring nameAlphaTestedPso = L"alphaTested";
    std::unique_ptr<GraphicPso> alphaTestedPso = std::make_unique<GraphicPso>(*mGraphicPsos.at(nameOpaquePso));
    alphaTestedPso->SetPixelShader(mShaderManager->GetPixelShader("alphaTested")->GetByteCode());
    alphaTestedPso->SetCullMode(D3D12_CULL_MODE_NONE);
    alphaTestedPso->Finalize(nameAlphaTestedPso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameAlphaTestedPso] = std::move(alphaTestedPso);

    //
    // MARK STENCIL MIRRORS PSO
    //
    std::wstring nameMarkStencilMirrorsPso = L"markStencilMirrors";
    std::unique_ptr<GraphicPso> markStencilMirrorsPso = std::make_unique<GraphicPso>(*mGraphicPsos.at(nameOpaquePso));
    CD3DX12_BLEND_DESC mirrorBlendState(D3D12_DEFAULT);
    mirrorBlendState.RenderTarget[0].RenderTargetWriteMask = 0;
    D3D12_DEPTH_STENCIL_DESC mirrorDSS;
    mirrorDSS.DepthEnable = true;
    mirrorDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    mirrorDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    mirrorDSS.StencilEnable = true;
    mirrorDSS.StencilReadMask = 0xff;
    mirrorDSS.StencilWriteMask = 0xff;
    mirrorDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    mirrorDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    mirrorDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
    mirrorDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    mirrorDSS.BackFace = mirrorDSS.FrontFace;  // Back face settings are the same as front face.
    markStencilMirrorsPso->SetBlendState(mirrorBlendState);
    markStencilMirrorsPso->SetDepthStencilState(mirrorDSS);
    markStencilMirrorsPso->Finalize(nameMarkStencilMirrorsPso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameMarkStencilMirrorsPso] = std::move(markStencilMirrorsPso);

    //
    // DRAW STENCIL REFLECTIONS PSO
    //
    std::wstring nameDrawStencilReflectionsPso = L"drawStencilReflections";
    std::unique_ptr<GraphicPso> drawStencilReflectionsPso = std::make_unique<GraphicPso>(*mGraphicPsos.at(nameOpaquePso));
    D3D12_DEPTH_STENCIL_DESC reflectionsDSS;
    reflectionsDSS.DepthEnable = true;
    reflectionsDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    reflectionsDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    reflectionsDSS.StencilEnable = true;
    reflectionsDSS.StencilReadMask = 0xff;
    reflectionsDSS.StencilWriteMask = 0xff;
    reflectionsDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    reflectionsDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    reflectionsDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    reflectionsDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
    reflectionsDSS.BackFace = reflectionsDSS.FrontFace;  // Back face settings are the same as front face.
    drawStencilReflectionsPso->SetDepthStencilState(reflectionsDSS);
    drawStencilReflectionsPso->SetCullMode(D3D12_CULL_MODE_BACK);
    drawStencilReflectionsPso->SetFrontCounterClockwise(true);
    drawStencilReflectionsPso->Finalize(nameDrawStencilReflectionsPso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameDrawStencilReflectionsPso] = std::move(drawStencilReflectionsPso);

    //
    // SHADOW PSO
    //
    std::wstring nameShadowPso = L"shadow";
    std::unique_ptr<GraphicPso> shadowPso = std::make_unique<GraphicPso>(*mGraphicPsos.at(nameTransparentPso));
    D3D12_DEPTH_STENCIL_DESC shadowDSS;
    shadowDSS.DepthEnable = true;
    shadowDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    shadowDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    shadowDSS.StencilEnable = true;
    shadowDSS.StencilReadMask = 0xff;
    shadowDSS.StencilWriteMask = 0xff;
    shadowDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    shadowDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    shadowDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
    shadowDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
    shadowDSS.BackFace = shadowDSS.FrontFace;  // Back face settings are the same as front face.
    shadowPso->SetDepthStencilState(shadowDSS);
    shadowPso->Finalize(nameShadowPso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameShadowPso] = std::move(shadowPso);

    //
    // TREE SPRITES PSO
    //
    std::wstring nameTreeSpritesPso = L"treeSprites";
    std::unique_ptr<GraphicPso> treeSpritesPso = std::make_unique<GraphicPso>(*mGraphicPsos.at(nameOpaquePso));
    treeSpritesPso->SetVertexShader(mShaderManager->GetVertexShader("treeSprite")->GetByteCode());
    treeSpritesPso->SetGeometryShader(mShaderManager->GetGeometryShader("treeSprite")->GetByteCode());
    treeSpritesPso->SetPixelShader(mShaderManager->GetPixelShader("treeSprite")->GetByteCode());
    treeSpritesPso->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
    treeSpritesPso->SetCullMode(D3D12_CULL_MODE_NONE);
    treeSpritesPso->Finalize(nameTreeSpritesPso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameTreeSpritesPso] = std::move(treeSpritesPso);

    //
    // GEOMETRY SUBDIVIDE PSO
    //
    std::wstring nameGeometrySubdividePso = L"geometrySubdivide";
    std::unique_ptr<GraphicPso> geometrySubdividePso = std::make_unique<GraphicPso>(*mGraphicPsos.at(nameOpaquePso));
    geometrySubdividePso->SetVertexShader(mShaderManager->GetVertexShader("geometrySubdivide")->GetByteCode());
    geometrySubdividePso->SetGeometryShader(mShaderManager->GetGeometryShader("geometrySubdivide")->GetByteCode());
    geometrySubdividePso->SetPixelShader(mShaderManager->GetPixelShader("geometrySubdivide")->GetByteCode());
    geometrySubdividePso->SetCullMode(D3D12_CULL_MODE_BACK);
    geometrySubdividePso->Finalize(nameGeometrySubdividePso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameGeometrySubdividePso] = std::move(geometrySubdividePso);

    //
    // GEOMETRY SUBDIVIDE WIREFRAME PSO
    //
    std::wstring nameGeometrySubdivideWireframePso = L"geometrySubdivideWireFrame";
    std::unique_ptr<GraphicPso> geometrySubdivideWireframePso = std::make_unique<GraphicPso>(*geometrySubdividePso);
    geometrySubdivideWireframePso->SetFillMode(D3D12_FILL_MODE_WIREFRAME);
    geometrySubdivideWireframePso->Finalize(nameGeometrySubdivideWireframePso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameGeometrySubdivideWireframePso] = std::move(geometrySubdivideWireframePso);

    //
    // WAVES RENDER PSO
    //
    std::wstring nameWavesRenderPso = L"wavesRender";
    std::unique_ptr<GraphicPso> wavesRenderPso = std::make_unique<GraphicPso>(*mGraphicPsos.at(nameTransparentPso));
    wavesRenderPso->SetVertexShader(mShaderManager->GetVertexShader("waves")->GetByteCode());
    wavesRenderPso->Finalize(nameWavesRenderPso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameWavesRenderPso] = std::move(wavesRenderPso);

    //
    // SKY PSO
    //
    std::wstring nameSkyPso = L"sky";
    std::unique_ptr<GraphicPso> skyPso = std::make_unique<GraphicPso>(*mGraphicPsos.at(nameOpaquePso));
    skyPso->SetCullMode(D3D12_CULL_MODE_NONE);
    skyPso->SetDepthFunc(D3D12_COMPARISON_FUNC_LESS_EQUAL);
    skyPso->SetVertexShader(mShaderManager->GetVertexShader("sky")->GetByteCode());
    skyPso->SetPixelShader(mShaderManager->GetPixelShader("sky")->GetByteCode());
    skyPso->Finalize(nameSkyPso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameSkyPso] = std::move(skyPso);

    //
    // SKY WIREFRAME PSO
    //
    std::wstring nameSkyWireframePso = L"skyWireframe";
    std::unique_ptr<GraphicPso> skyWireframePso = std::make_unique<GraphicPso>(*skyPso);
    skyWireframePso->SetFillMode(D3D12_FILL_MODE_WIREFRAME);
    skyWireframePso->Finalize(nameSkyWireframePso, mInputLauoutManager->GetInputLayout(L"base"));
    mGraphicPsos[nameSkyWireframePso] = std::move(skyWireframePso);

    //
    // COMPOSITE PSO
    //
    std::wstring nameCompositePso = L"composite";
    std::unique_ptr<GraphicPso> compositePso = std::make_unique<GraphicPso>(*mGraphicPsos.at(nameOpaquePso));
    compositePso->SetRootSignature(mRootSignatureManager->GetRootSignature(L"postProcess"));
    compositePso->SetDepthStencilState(CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT));
    compositePso->SetDepthEnable(false);
    compositePso->SetDepthWriteMask(D3D12_DEPTH_WRITE_MASK_ZERO);
    compositePso->SetDepthFunc(D3D12_COMPARISON_FUNC_ALWAYS);
    compositePso->SetVertexShader(mShaderManager->GetVertexShader("composite")->GetByteCode());
    compositePso->SetPixelShader(mShaderManager->GetPixelShader("composite")->GetByteCode());
    compositePso->Finalize(nameCompositePso, nullptr);  // No input layout for post-process.
    mGraphicPsos[nameCompositePso] = std::move(compositePso);
}

void PsoManager::BuildComputePsos() 
{

}
