#pragma once
#include "GraphicCommonHeaders.h"

class RootSignature;

enum class EShaderType
{
    Vertex,
    Pixel,
    Geometry,
    Hull,
    Domain,
    Uninitialized
};


class Shader
{
	Microsoft::WRL::ComPtr<ID3DBlob> mBlob = nullptr;
    std::wstring mName{};
    std::wstring mPath{};
    EShaderType mType = EShaderType::Uninitialized;
    RootSignature* pUsableRootSignature = nullptr;

public:
    Shader(std::wstring);

};