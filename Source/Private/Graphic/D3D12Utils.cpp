#include "D3D12Utils.h"
#include <fstream>
#include <filesystem>

using namespace Microsoft::WRL;
using namespace Kds::App;

ComPtr<ID3D12Resource> D3D12Utils::CreateDefaultBuffer(
    ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer)
{
    ComPtr<ID3D12Resource> defaultBuffer;
    const auto ResDescBuf = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    // Create the actual default buffer resource.
    const auto HeapPropDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    device->CreateCommittedResource(&HeapPropDefault, D3D12_HEAP_FLAG_NONE, &ResDescBuf, D3D12_RESOURCE_STATE_COMMON, nullptr,
        IID_PPV_ARGS(defaultBuffer.GetAddressOf())) >> Check;

    // In order to copy CPU memory data into our default buffer, we need to create
    // an intermediate upload heap.
    const auto HeapPropUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    device->CreateCommittedResource(&HeapPropUpload, D3D12_HEAP_FLAG_NONE, &ResDescBuf, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(uploadBuffer.GetAddressOf())) >> Check;

    // Describe the data we want to copy into the default buffer.
    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData = initData;
    subResourceData.RowPitch = byteSize;
    subResourceData.SlicePitch = subResourceData.RowPitch;

    // Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
    // will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
    // the intermediate upload heap data will be copied to mBuffer.

    const auto ResBarStateToCopy =
        CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

    cmdList->ResourceBarrier(1, &ResBarStateToCopy);
    UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

    const auto ResBarCopyToState =
        CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
    cmdList->ResourceBarrier(1, &ResBarCopyToState);

    // Note: uploadBuffer has to be kept alive after the above function calls because
    // the command list has not been executed yet that performs the actual copy.
    // The caller can Release the uploadBuffer after it knows the copy has been executed.

    return defaultBuffer;
}

ComPtr<ID3DBlob> D3D12Utils::LoadBinary(const std::string& filename)
{
    std::ifstream fin(filename, std::ios::binary);

    fin.seekg(0, std::ios_base::end);
    std::ifstream::pos_type size = static_cast<size_t>(fin.tellg());
    fin.seekg(0, std::ios_base::beg);

    ComPtr<ID3DBlob> blob;
    D3DCreateBlob(size, blob.GetAddressOf()) >> Check;

    fin.read(static_cast<char*>(blob->GetBufferPointer()), size);
    fin.close();

    return blob;
}

ComPtr<ID3DBlob> D3D12Utils::CompileShader(
    const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG_NAME_FOR_SOURCE;
#endif

    HRESULT hr = S_OK;

    ComPtr<ID3DBlob> byteCode = nullptr;
    ComPtr<ID3DBlob> errors;
    hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(), compileFlags,
        0, &byteCode, &errors);

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

    // Now write the contents of the blob pPDB to a file named the value of pName
    // Not illustrated here

    // Now remove the debug info from the target shader, resulting in a smaller shader
    // in your final application’s data:
    ComPtr<ID3DBlob> pStripped;

    D3DStripShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), D3DCOMPILER_STRIP_DEBUG_INFO, pStripped.GetAddressOf()) >>
        Check;

    // Finally, write the contents of pStripped as your final shader file.
    if (SUCCEEDED(hr))
    {
        std::filesystem::path pdbPath(L"bin/Debug/");
        pdbPath.replace_extension().replace_filename(pName);

        std::ofstream outFile(pdbPath, std::ios::binary);
        outFile.write(static_cast<const char*>(pPDB->GetBufferPointer()), pPDB->GetBufferSize());
    }
#endif  

    return byteCode;
}
