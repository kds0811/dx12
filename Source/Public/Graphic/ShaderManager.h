#pragma once
#include "GraphicCommonHeaders.h"
#include <memory>
#include <unordered_map>
#include <string>

class Shader;

class ShaderManager
{
    using UnMapStringUP = std::unordered_map<std::string, std::unique_ptr<Shader>>;

    UnMapStringUP mVertexShaders;
    UnMapStringUP mPixelShaders;
    UnMapStringUP mComputeShaders;
    UnMapStringUP mGeometryShaders;
    UnMapStringUP mHullShaders;
    UnMapStringUP mDomainShaders;

    const std::wstring mShadersFolderPath = L"..\\Source\\Shaders\\";

    const std::string mVSEntryPoint = "VS";
    const std::string mPSEntryPoint = "PS";
    const std::string mGSEntryPoint = "GS";
    const std::string mHSEntryPoint = "HS";
    const std::string mDSEntryPoint = "DS";

    const std::string mVSTarget = "vs_6_0";
    const std::string mPSTarget = "ps_6_0";
    const std::string mCSTarget = "cs_6_0";
    const std::string mGSTarget = "gs_6_0";
    const std::string mHSTarget = "hs_6_0";
    const std::string mDSTarget = "ds_6_0";

public:
    ShaderManager();
    ~ShaderManager();

    const Shader* GetVertexShader(const std::string& name) const;
    const Shader* GetPixelShader(const std::string& name) const;
    const Shader* GetComputeShader(const std::string& name) const;
    const Shader* GetHullShader(const std::string& name) const;
    const Shader* GetDomainShader(const std::string& name) const;


private:
    void BuildShaders();
    void BuildVertexShaders();
    void BuildPixelShaders();
    void BuildComputeShaders();
    void BuildGeometryShaders();
    void BuildHullShaders();
    void BuildDomainShaders();
    
    bool CheckContains(const UnMapStringUP& map, const std::string& name) const;


};