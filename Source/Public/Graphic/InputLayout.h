#pragma once
#include "GraphicCommonHeaders.h"
#include <vector>

class InputLayout
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout{};

public:
    InputLayout(int size);
    void AddInputElementDesc(D3D12_INPUT_ELEMENT_DESC layout);

    D3D12_INPUT_LAYOUT_DESC GetInputLayoutDescriptor() const;
};