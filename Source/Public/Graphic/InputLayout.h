#pragma once
#include "GraphicCommonHeaders.h"
#include <vector>

class InputLayout
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout{};
    D3D12_INPUT_LAYOUT_DESC mInputLayoutDesc;

public:
    InputLayout(int size);
    void AddInputElementDesc(D3D12_INPUT_ELEMENT_DESC layout);
    void Finalize();

    const D3D12_INPUT_LAYOUT_DESC& GetInputLayoutDescriptor() const;
};