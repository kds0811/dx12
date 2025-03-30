#pragma once
#include "GraphicCommonHeaders.h"

class Pso
{
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso;

public:
    Pso();
    [[nodiscard]] inline ID3D12PipelineState* GetPso() noexcept { return mPso.Get(); }

};