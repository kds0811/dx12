#pragma once
#include "D3D12Utils.h"
#include <memory>
#include <unordered_map>
#include <string>
#include "Material.h"
#include "Texture.h"


class MaterialBuilder
{

public:
    std::unordered_map<std::string, std::unique_ptr<Material>> CreateMaterials(std::unordered_map<std::string, std::unique_ptr<Texture>>& textures);

};