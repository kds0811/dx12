#pragma once
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

public:
    ShaderManager();
};