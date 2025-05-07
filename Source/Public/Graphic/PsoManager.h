#pragma once
#include "GraphicCommonHeaders.h"
#include <memory>
#include <unordered_map>

class Pso;
class RootSignatureManager;
class ShaderManager;

class PsoManager
{
    using PsoMap = std::unordered_map<std::string, std::unique_ptr<Pso>>;

    std::unique_ptr<RootSignatureManager> mRootSignatureManager = nullptr;
    std::unique_ptr<ShaderManager> mShaderManager = nullptr;
    PsoMap mPsos{};

public:
    PsoManager();
    ~PsoManager();


private:
    void BuildPso();
};