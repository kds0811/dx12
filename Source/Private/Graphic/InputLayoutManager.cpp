#include "InputLayoutManager.h"
#include "InputLayout.h"

InputLayoutManager::InputLayoutManager()
{
    BuildInputLayouts();
}

InputLayoutManager::~InputLayoutManager() {}

void InputLayoutManager::BuildInputLayouts()
{
    BuildStandartInputLayout();
}

void InputLayoutManager::BuildStandartInputLayout() 
{
    std::unique_ptr<InputLayout> standartLayout = std::make_unique<InputLayout>();
    standartLayout->AddInputElementDesc({"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    standartLayout->AddInputElementDesc({"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    standartLayout->AddInputElementDesc({"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    mInputLayouts["standartInputLayout"] = std::move(standartLayout);
}

void InputLayoutManager::BuildTreeSpriteInputLayout()
{
    std::unique_ptr<InputLayout> treeSpritetLayout = std::make_unique<InputLayout>();
    treeSpritetLayout->AddInputElementDesc({"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    treeSpritetLayout->AddInputElementDesc({"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    mInputLayouts["treeSpriteInputLayout"] = std::move(treeSpritetLayout);
}


