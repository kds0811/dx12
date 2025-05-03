#pragma once
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

private:
    void BuildInputLayouts();
};