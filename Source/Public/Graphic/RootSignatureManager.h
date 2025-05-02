#pragma once
#include <memory>
#include <unordered_map>
#include <string>

class RootSignature;

class RootSignatureManager
{
    static constexpr size_t mStandrartRootSigSize = 5;
    static constexpr size_t mPostProcRootSigSize = 3;
    static constexpr size_t mBilateralBlurRootSigSize = 3;
    std::unordered_map<std::wstring, std::unique_ptr<RootSignature>> mRootSignatures;

public:
    RootSignatureManager();
    ~RootSignatureManager();

    RootSignature* GetRootSignature(const std::wstring& name);
    const RootSignature* GetRootSignature(const std::wstring& name) const;

private:
    void BuildRootSignatures();
    void BuildBaseRootSignature();
    void BuildPostProcessRootSignature();
    void BuildBilateralBlurRootSignature();

    bool CheckContains(const std::wstring& name) const;
};
