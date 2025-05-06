#pragma once
#include "GraphicCommonHeaders.h"
#include <memory>
#include <unordered_map>
#include <string>

class Shader;

class ShaderManager
{
    std::unordered_map<std::string, std::unique_ptr<Shader>> mVertexShaders;
    std::unordered_map<std::string, std::unique_ptr<Shader>> mPixelShaders;
    std::unordered_map<std::string, std::unique_ptr<Shader>> mComputeShaders;
    std::unordered_map<std::string, std::unique_ptr<Shader>> mGeometryShaders;
    std::unordered_map<std::string, std::unique_ptr<Shader>> mHullShaders;
    std::unordered_map<std::string, std::unique_ptr<Shader>> mDomainShaders;

    std::wstring mShadersFolderPath = L"..\\Source\\Shaders\\";

    std::string mVSEntryPoint = "VS";
    std::string mPSEntryPoint = "PS";
    std::string mGSEntryPoint = "GS";
    std::string mHSEntryPoint = "HS";
    std::string mDSEntryPoint = "DS";

    std::string mVSTarget = "vs_6_0";
    std::string mPSTarget = "ps_6_0";
    std::string mCSTarget = "cs_6_0";
    std::string mGSTarget = "gs_6_0";
    std::string mHSTarget = "hs_6_0";
    std::string mDSTarget = "ds_6_0";

public:
    ShaderManager();
    ~ShaderManager();

private:
    void BuildShaders();
    void BuildVertexShaders();
    void BuildPixelShaders();
    void BuildComputeShaders();
    void BuildGeometryShaders();
    void BuildHullShaders();
    void BuildDomainShaders();
};