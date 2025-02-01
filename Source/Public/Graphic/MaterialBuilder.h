#pragma once
#include "D3D12Utils.h"
#include <memory>
#include <unordered_map>

class MaterialBuilder
{
public:
    std::unordered_map<EMaterialType, std::unique_ptr<Material>> CreateMaterials();
};