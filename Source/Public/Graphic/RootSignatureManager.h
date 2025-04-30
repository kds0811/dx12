#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "RootSignature.h"


class RootSignatureManager
{
    static constexpr size_t mStandrartRootSigSize = 5;
    static constexpr size_t mPostProcRootSigSize = 3;
    static constexpr size_t mBilateralBlurRootSigSize = 3;
    std::unique_ptr<RootSignature<mStandrartRootSigSize>> mStandartRootSignature;
    std::unique_ptr<RootSignature<mPostProcRootSigSize>> mPostProcRootSignature;
    std::unique_ptr<RootSignature<mBilateralBlurRootSigSize>> mBilateralBlurRootSignature;

public:
    RootSignatureManager();
    ~RootSignatureManager();

private:
    void BuildRootSignatures();
    void BuildBaseRootSignature();
    void BuildPostProcessRootSignature();
    void BuildBilateralBlurRootSignature();
};
