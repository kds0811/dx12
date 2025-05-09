#include "InputLayout.h"

InputLayout::InputLayout(int size) 
{
    if (size > mInputLayout.capacity())
    {
        mInputLayout.reserve(size);
    }
}

void InputLayout::AddInputElementDesc(D3D12_INPUT_ELEMENT_DESC layout)
{
    mInputLayout.push_back(layout);
}

void InputLayout::Finalize()
{
    mInputLayoutDesc = D3D12_INPUT_LAYOUT_DESC{mInputLayout.data(), static_cast<UINT>(mInputLayout.size())};
}

const D3D12_INPUT_LAYOUT_DESC& InputLayout::GetInputLayoutDescriptor() const
{
    return mInputLayoutDesc;
}
