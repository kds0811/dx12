#pragma once
#include "GraphicCommonHeaders.h"
#include <memory>
#include <unordered_map>

class GraphicPso;
class ComputePso;
class RootSignatureManager;
class ShaderManager;
class InputLayoutManager;

class PsoManager
{
    using GraphicPsoUnMap = std::unordered_map<std::wstring, std::unique_ptr<GraphicPso>>;
    using ComputePsoUnMap = std::unordered_map<std::wstring, std::unique_ptr<ComputePso>>;

    std::unique_ptr<RootSignatureManager> mRootSignatureManager = nullptr;
    std::unique_ptr<ShaderManager> mShaderManager = nullptr;
    std::unique_ptr<InputLayoutManager> mInputLauoutManager = nullptr;
    GraphicPsoUnMap mGraphicPsos{};
    ComputePsoUnMap mComputePsos{};

public:
    PsoManager();
    ~PsoManager();

    GraphicPso* GetGraphicPso(const std::wstring& name);
    ComputePso* GetComputePso(const std::wstring& name);
    const GraphicPso* GetGraphicPso(const std::wstring& name) const;
    const ComputePso* GetComputePso(const std::wstring& name) const;

private:
    bool HasGraphicPso(const std::wstring& name) const;
    bool HasComputePso(const std::wstring& name) const;
    void BuildPso();
    void BuildGraphicPsos();
    void BuildComputePsos();
};