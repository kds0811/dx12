#pragma once
#include "GraphicCommonHeaders.h"
#include <memory>

class Pso;
class RootSignatureManager;

class PsoManager
{
    static inline std::unique_ptr<Pso> mStandartPso = nullptr;
    std::unique_ptr<RootSignatureManager> mRootSignatureManager = nullptr;

public:
    PsoManager();
    ~PsoManager();

    static inline Pso* GetStandartPso() noexcept { return mStandartPso.get(); }

private:
    void Initialize();
};