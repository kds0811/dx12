#include "ShaderManager.h"
#include "Shader.h"

static constexpr D3D_SHADER_MACRO FogDefines[] = {"FOG", "1", NULL, NULL};
static constexpr D3D_SHADER_MACRO AlphaTestDefines[] = {"FOG", "1", "ALPHA_TEST", "1", NULL, NULL};
static constexpr D3D_SHADER_MACRO WaveDefines[] = {"DISPLACEMENT_MAP", "1", NULL, NULL};

ShaderManager::ShaderManager()
{
    BuildShaders();
}

ShaderManager::~ShaderManager() = default;

const Shader* ShaderManager::GetVertexShader(const std::string& name) const
{
    if (CheckContains(mVertexShaders, name))
    {
        return mVertexShaders.at(name).get();
    }
    return nullptr;
}

const Shader* ShaderManager::GetPixelShader(const std::string& name) const
{
    if (CheckContains(mPixelShaders, name))
    {
        return mPixelShaders.at(name).get();
    }
    return nullptr;
}

const Shader* ShaderManager::GetComputeShader(const std::string& name) const
{
    if (CheckContains(mComputeShaders, name))
    {
        return mComputeShaders.at(name).get();
    }
    return nullptr;
}

const Shader* ShaderManager::GetHullShader(const std::string& name) const
{
    if (CheckContains(mHullShaders, name))
    {
        return mHullShaders.at(name).get();
    }
    return nullptr;
}

const Shader* ShaderManager::GetDomainShader(const std::string& name) const
{
    if (CheckContains(mDomainShaders, name))
    {
        return mDomainShaders.at(name).get();
    }
    return nullptr;
}

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
    mVertexShaders["standard"] = std::make_unique<Shader>(mShadersFolderPath + L"Default.hlsl", nullptr, mVSEntryPoint, mVSTarget);
    mVertexShaders["treeSprite"] = std::make_unique<Shader>(mShadersFolderPath + L"ThreeSprite.hlsl", nullptr, mVSEntryPoint, mVSTarget);
    mVertexShaders["geometrySubdivide"] = std::make_unique<Shader>(mShadersFolderPath + L"GeometrySubdivide.hlsl", nullptr, mVSEntryPoint, mVSTarget);
    mVertexShaders["waves"] = std::make_unique<Shader>(mShadersFolderPath + L"Default.hlsl", WaveDefines, mVSEntryPoint, mVSTarget);
    mVertexShaders["composite"] = std::make_unique<Shader>(mShadersFolderPath + L"Composite.hlsl", nullptr, mVSEntryPoint, mVSTarget);
    mVertexShaders["tess"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTri.hlsl", nullptr, mVSEntryPoint, mVSTarget);
    mVertexShaders["tessFracOdd"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTriFracOdd.hlsl", nullptr, mVSEntryPoint, mVSTarget);
    mVertexShaders["tessFracEven"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTriFracEven.hlsl", nullptr, mVSEntryPoint, mVSTarget);
    mVertexShaders["tessPow"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTriPow.hlsl", nullptr, mVSEntryPoint, mVSTarget);
    mVertexShaders["sky"] = std::make_unique<Shader>(mShadersFolderPath + L"Sky.hlsl", nullptr, mVSEntryPoint, mVSTarget);
}

void ShaderManager::BuildPixelShaders()
{
    mPixelShaders["opaque"] = std::make_unique<Shader>(mShadersFolderPath + L"Default.hlsl", nullptr, mPSEntryPoint, mPSTarget);
    mPixelShaders["alphaTested"] = std::make_unique<Shader>(mShadersFolderPath + L"Default.hlsl", AlphaTestDefines, mPSEntryPoint, mPSTarget);
    mPixelShaders["treeSprite"] = std::make_unique<Shader>(mShadersFolderPath + L"ThreeSprite.hlsl", AlphaTestDefines, mPSEntryPoint, mPSTarget);
    mPixelShaders["geometrySubdivide"] = std::make_unique<Shader>(mShadersFolderPath + L"GeometrySubdivide.hlsl", FogDefines, mPSEntryPoint, mPSTarget);
    mPixelShaders["composite"] = std::make_unique<Shader>(mShadersFolderPath + L"Composite.hlsl", nullptr, mPSEntryPoint, mPSTarget);
    mPixelShaders["tess"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTri.hlsl", nullptr, mPSEntryPoint, mPSTarget);
    mPixelShaders["tessFracOdd"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTriFracOdd.hlsl", nullptr, mPSEntryPoint, mPSTarget);
    mPixelShaders["tessFracEven"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTriFracEven.hlsl", nullptr, mPSEntryPoint, mPSTarget);
    mPixelShaders["tessPow"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTriPow.hlsl", nullptr, mPSEntryPoint, mPSTarget);
    mPixelShaders["sky"] = std::make_unique<Shader>(mShadersFolderPath + L"Sky.hlsl", nullptr, mPSEntryPoint, mPSTarget);
}

void ShaderManager::BuildComputeShaders()
{
    mComputeShaders["horzBlur"] = std::make_unique<Shader>(mShadersFolderPath + L"Blur.hlsl", nullptr, "HorzBlurCS", mCSTarget);
    mComputeShaders["vertBlur"] = std::make_unique<Shader>(mShadersFolderPath + L"Blur.hlsl", nullptr, "VertBlurCS", mCSTarget);
    mComputeShaders["horzBilateral"] = std::make_unique<Shader>(mShadersFolderPath + L"BilateralFilter.hlsl", nullptr, "HorzBilateralCS", mCSTarget);
    mComputeShaders["vertBilateral"] = std::make_unique<Shader>(mShadersFolderPath + L"BilateralFilter.hlsl", nullptr, "VertBilateralCS", mCSTarget);
    mComputeShaders["wavesUpdate"] = std::make_unique<Shader>(mShadersFolderPath + L"WaveSim.hlsl", nullptr, "UpdateWavesCS", mCSTarget);
    mComputeShaders["wavesDisturb"] = std::make_unique<Shader>(mShadersFolderPath + L"WaveSim.hlsl", nullptr, "DisturbWavesCS", mCSTarget);
    mComputeShaders["sobel"] = std::make_unique<Shader>(mShadersFolderPath + L"Sobel.hlsl", nullptr, "SobelCS", mCSTarget);
}

void ShaderManager::BuildGeometryShaders()
{
    mGeometryShaders["treeSprite"] = std::make_unique<Shader>(mShadersFolderPath + L"ThreeSprite.hlsl", nullptr, mGSEntryPoint, mGSTarget);
    mGeometryShaders["geometrySubdivide"] = std::make_unique<Shader>(mShadersFolderPath + L"GeometrySubdivide.hlsl", nullptr, mGSEntryPoint, mGSTarget);
}

void ShaderManager::BuildHullShaders()
{
    mHullShaders["tess"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTri.hlsl", nullptr, mHSEntryPoint, mHSTarget);
    mHullShaders["tessFracOdd"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTriFracOdd.hlsl", nullptr, mHSEntryPoint, mHSTarget);
    mHullShaders["tessFracEven"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTriFracEven.hlsl", nullptr, mHSEntryPoint, mHSTarget);
    mHullShaders["tessPow"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTriPow.hlsl", nullptr, mHSEntryPoint, mHSTarget);
}

void ShaderManager::BuildDomainShaders()
{
    mDomainShaders["tess"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTri.hlsl", nullptr, mDSEntryPoint, mDSTarget);
    mDomainShaders["tessFracOdd"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTriFracOdd.hlsl", nullptr, mDSEntryPoint, mDSTarget);
    mDomainShaders["tessFracEven"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTriFracEven.hlsl", nullptr, mDSEntryPoint, mDSTarget);
    mDomainShaders["tessPow"] = std::make_unique<Shader>(mShadersFolderPath + L"TessellationTriPow.hlsl", nullptr, mDSEntryPoint, mDSTarget);
}

bool ShaderManager::CheckContains(const UnMapStringUP& map, const std::string& name) const
{
    if (map.contains(name))
    {
        return true;
    }
    LOG_ERROR("Shader ", name, " Is not in container");
    return false;
}
