#include "ShaderManager.h"
#include "Shader.h"

static constexpr D3D_SHADER_MACRO defines[] = {"FOG", "1", NULL, NULL};
static constexpr D3D_SHADER_MACRO alphaTestDefines[] = {"FOG", "1", "ALPHA_TEST", "1", NULL, NULL};
static constexpr D3D_SHADER_MACRO waveDefines[] = {"DISPLACEMENT_MAP", "1", NULL, NULL};

ShaderManager::ShaderManager()
{
    BuildShaders();
}

ShaderManager::~ShaderManager() = default;

void ShaderManager::BuildShaders()
{
    BuildVertexShaders();
    BuildPixelShaders();
    BuildComputeShaders();
    BuildGeometryShaders();
    BuildHullShaders();
    BuildDomainShaders();
}

void ShaderManager::BuildVertexShaders()
{
    mVertexShaders["standardVS"] = std::make_unique<Shader>(L"..\\Source\\Shaders\\Default.hlsl", nullptr, mVertexEntryPoint, mVertexTarget);
    mVertexShaders["treeSpriteVS"] = std::make_unique<Shader>(L"..\\Source\\Shaders\\ThreeSprite.hlsl", nullptr, mVertexEntryPoint, mVertexTarget);
    mVertexShaders["geometrySubdivideVS"] = std::make_unique<Shader>(L"..\\Source\\Shaders\\GeometrySubdivide.hlsl", nullptr, mVertexEntryPoint, mVertexTarget);

    mShaders["wavesVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Default.hlsl", waveDefines, "VS", "vs_5_1");
    mShaders["compositeVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Composite.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["tessVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTri.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["tessFracOddVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracOdd.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["tessFracEvenVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracEven.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["tessPowVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriPow.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["skyVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Sky.hlsl", nullptr, "VS", "vs_5_1");
}

void ShaderManager::BuildPixelShaders()
{

    mShaders["opaquePS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Default.hlsl", defines, "PS", "ps_5_1");
    mShaders["alphaTestedPS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Default.hlsl", alphaTestDefines, "PS", "ps_5_1");
    mShaders["treeSpritePS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\ThreeSprite.hlsl", alphaTestDefines, "PS", "ps_5_1");
    mShaders["geometrySubdividePS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\GeometrySubdivide.hlsl", defines, "PS", "ps_5_1");
    mShaders["compositePS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Composite.hlsl", nullptr, "PS", "ps_5_1");
    mShaders["tessPS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTri.hlsl", nullptr, "PS", "ps_5_1");
    mShaders["tessFracOddPS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracOdd.hlsl", nullptr, "PS", "ps_5_1");
    mShaders["tessFracEvenPS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracEven.hlsl", nullptr, "PS", "ps_5_1");
    mShaders["tessPowPS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriPow.hlsl", nullptr, "PS", "ps_5_1");
    mShaders["skyPS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Sky.hlsl", nullptr, "PS", "ps_5_1");
}

void ShaderManager::BuildComputeShaders()
{
    mShaders["horzBlurCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Blur.hlsl", nullptr, "HorzBlurCS", "cs_5_1");
    mShaders["vertBlurCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Blur.hlsl", nullptr, "VertBlurCS", "cs_5_1");

    mShaders["horzBilateralCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\BilateralFilter.hlsl", nullptr, "HorzBilateralCS", "cs_5_1");
    mShaders["vertBilateralCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\BilateralFilter.hlsl", nullptr, "VertBilateralCS", "cs_5_1");

    mShaders["wavesUpdateCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\WaveSim.hlsl", nullptr, "UpdateWavesCS", "cs_5_1");
    mShaders["wavesDisturbCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\WaveSim.hlsl", nullptr, "DisturbWavesCS", "cs_5_1");
    mShaders["sobelCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Sobel.hlsl", nullptr, "SobelCS", "cs_5_1");
}

void ShaderManager::BuildGeometryShaders()
{
    mShaders["treeSpriteGS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\ThreeSprite.hlsl", nullptr, "GS", "gs_5_1");
    mShaders["geometrySubdivideGS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\GeometrySubdivide.hlsl", nullptr, "GS", "gs_5_1");
}

void ShaderManager::BuildHullShaders()
{
    mShaders["tessHS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTri.hlsl", nullptr, "HS", "hs_5_1");
    mShaders["tessFracOddHS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracOdd.hlsl", nullptr, "HS", "hs_5_1");
    mShaders["tessFracEvenHS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracEven.hlsl", nullptr, "HS", "hs_5_1");
    mShaders["tessPowHS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriPow.hlsl", nullptr, "HS", "hs_5_1");
}

void ShaderManager::BuildDomainShaders()
{
    mShaders["tessDS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTri.hlsl", nullptr, "DS", "ds_5_1");
    mShaders["tessFracOddDS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracOdd.hlsl", nullptr, "DS", "ds_5_1");
    mShaders["tessFracEvenDS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracEven.hlsl", nullptr, "DS", "ds_5_1");
    mShaders["tessPowDS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriPow.hlsl", nullptr, "DS", "ds_5_1");
}
