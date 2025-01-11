#include "D3D12Utils.h"
#include <fstream>

using namespace Microsoft::WRL;

ComPtr<ID3D12Resource> D3D12Utils::CreateDefaultBuffer(
    ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
    const void* initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer)
{
    ComPtr<ID3D12Resource> defaultBuffer;
    const auto ResDescBuf = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    // Create the actual default buffer resource.
    const auto HeapPropDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    device->CreateCommittedResource(&HeapPropDefault, D3D12_HEAP_FLAG_NONE, &ResDescBuf, D3D12_RESOURCE_STATE_COMMON, nullptr,
        IID_PPV_ARGS(defaultBuffer.GetAddressOf())) >>
        Kds::App::Check;

    // In order to copy CPU memory data into our default buffer, we need to create
    // an intermediate upload heap.
    const auto HeapPropUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    device->CreateCommittedResource(&HeapPropUpload, D3D12_HEAP_FLAG_NONE, &ResDescBuf, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(uploadBuffer.GetAddressOf())) >>
        Kds::App::Check;

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


ComPtr<ID3DBlob>D3D12Utils::LoadBinary(const std::string& filename)
{
    std::ifstream fin(filename, std::ios::binary);

    fin.seekg(0, std::ios_base::end);
    std::ifstream::pos_type size = static_cast<size_t>(fin.tellg());
    fin.seekg(0, std::ios_base::beg);

   ComPtr<ID3DBlob> blob;
    D3DCreateBlob(size, blob.GetAddressOf()) >> Kds::App::Check;

    fin.read(static_cast<char*>(blob->GetBufferPointer()), size);
    fin.close();

    return blob;
}

ComPtr<ID3DBlob> D3D12Utils::CompileShader(
    const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = S_OK;

    ComPtr<ID3DBlob> byteCode = nullptr;
    ComPtr<ID3DBlob> errors;
    hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(), compileFlags,
        0, &byteCode, &errors);

    if (errors != nullptr) OutputDebugStringA((char*)errors->GetBufferPointer());

    hr >> Kds::App::Check;

    return byteCode;
}
