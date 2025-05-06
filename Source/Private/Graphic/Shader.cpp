#include "Shader.h"
#include <fstream>
#include <filesystem>

using namespace Microsoft::WRL;
using namespace Kds::App;

Shader::Shader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target) 
{
    CompileShader(filename, defines, entrypoint, target);
}

D3D12_SHADER_BYTECODE Shader::GetByteCode() const
{
    return D3D12_SHADER_BYTECODE{reinterpret_cast<BYTE*>(mBlob->GetBufferPointer()), mBlob->GetBufferSize()};
}


void Shader::CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target) 
{
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG_NAME_FOR_SOURCE;
#endif

    HRESULT hr = S_OK;

    ComPtr<ID3DBlob> byteCode = nullptr;
    ComPtr<ID3DBlob> errors;
    hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

    if (errors)
    {
        OutputDebugStringA((char*)errors->GetBufferPointer());
    }
    hr >> Check;

// save PDB file for debug on PIX
#if defined(DEBUG) || defined(_DEBUG)

    // Starting with a compiled shader in pShaderBytecode of length BytecodeLength,
    // retrieve the debug info part of the shader:
    ComPtr<ID3DBlob> pPDB;
    D3DGetBlobPart(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), D3D_BLOB_PDB, 0, pPDB.GetAddressOf()) >> Check;

    // Now retrieve the suggested name for the debug data file:
    ComPtr<ID3DBlob> pPDBName;
    D3DGetBlobPart(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), D3D_BLOB_DEBUG_NAME, 0, pPDBName.GetAddressOf()) >> Check;

    // This struct represents the first four bytes of the name blob:
    struct ShaderDebugName
    {
        uint16_t Flags;       // Reserved, must be set to zero.
        uint16_t NameLength;  // Length of the debug name, without null terminator.
                              // Followed by NameLength bytes of the UTF-8-encoded name.
                              // Followed by a null terminator.
                              // Followed by [0-3] zero bytes to align to a 4-byte boundary.
    };

    auto pDebugNameData = reinterpret_cast<const ShaderDebugName*>(pPDBName->GetBufferPointer());
    auto pName = reinterpret_cast<const char*>(pDebugNameData + 1);

    // Now remove the debug info from the target shader, resulting in a smaller shader
    // in your final application’s data:
    ComPtr<ID3DBlob> pStripped;

    D3DStripShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), D3DCOMPILER_STRIP_DEBUG_INFO, pStripped.GetAddressOf()) >> Check;

    // Finally, write the contents of pStripped as your final shader file.
    if (SUCCEEDED(hr))
    {
        std::filesystem::path pdbPath(L"bin/Debug/");
        pdbPath.replace_extension().replace_filename(pName);

        std::ofstream outFile(pdbPath, std::ios::binary);
        outFile.write(static_cast<const char*>(pPDB->GetBufferPointer()), pPDB->GetBufferSize());
    }
#endif

    mBlob = byteCode;
}
