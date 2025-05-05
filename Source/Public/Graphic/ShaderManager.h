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

    std::string mVertexEntryPoint = {"VS"};
    std::string mVertexTarget = {"vs_5_1"};



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