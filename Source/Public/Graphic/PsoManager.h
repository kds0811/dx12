#pragma once
#include "GraphicCommonHeaders.h"

class Pso;

class PsoManager
{
    static inline std::unique_ptr<Pso> mStandartPso = nullptr;

public:
    PsoManager();
    ~PsoManager();

    static inline Pso* GetStandartPso() noexcept { return mStandartPso.get(); }

private:
    void Initialize();
};