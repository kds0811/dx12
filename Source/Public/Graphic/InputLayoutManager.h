#pragma once
#include "GraphicCommonHeaders.h"
#include <memory>
#include <unordered_map>
#include <string>

class InputLayout;

class InputLayoutManager
{
    std::unordered_map<std::wstring, std::unique_ptr<InputLayout>> mInputLayouts;

public:
    InputLayoutManager();
    ~InputLayoutManager();
    InputLayoutManager(const InputLayoutManager&) = delete;
    InputLayoutManager& operator=(const InputLayoutManager&) = delete;
    InputLayoutManager(const InputLayoutManager&&) noexcept = delete;
    InputLayoutManager& operator=(const InputLayoutManager&&) noexcept = delete;

    const InputLayout* GetInputLayout(const std::wstring& name) const;

private:
    void BuildInputLayouts();
    void BuildStandartInputLayout();
    void BuildTreeSpriteInputLayout();
};