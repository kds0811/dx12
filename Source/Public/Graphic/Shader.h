#pragma once
#include "GraphicCommonHeaders.h"
#include <string>

class Shader
{
    Microsoft::WRL::ComPtr<ID3DBlob> mBlob = nullptr;

public:
    Shader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);

    D3D12_SHADER_BYTECODE GetByteCode() const;

private:
    void CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);
};