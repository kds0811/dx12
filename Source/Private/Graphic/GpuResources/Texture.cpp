#include "Texture.h"
#include "GraphicError.h"
#include "DDSTextureLoader.h"
#include "Logger.h"
#include "Device.h"
#include "CommandList.h"
#include "DescriptorHeapManager.h"

Texture::Texture(ETextureType type, const std::wstring& name, std::wstring fileName, CommandList* cmdList)
	:
	mType(type),
	mFilename(std::move(fileName))
{
    LoadFromFile(name, cmdList);
}

void Texture::LoadFromFile(const std::wstring& name, CommandList* cmdList)
{
    Microsoft::WRL::ComPtr<ID3D12Resource> tempRes;
    DirectX::CreateDDSTextureFromFile12(Device::GetDevice(), cmdList->GetCommandList(), mFilename.c_str(), tempRes, mUploadHeap) >> Kds::App::Check;
   
	auto desc = tempRes->GetDesc();
    mFormat = desc.Format;
    mWidth = desc.Width;
    mHeight = desc.Height;

    
    SetResource(name, tempRes.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    BuildDescriptors();
}


void Texture::BuildDescriptors()
{
    mSrvDescriptorHandle = DescriptorHeapManager::AllocateCbvSrvUav();
    if (mSrvDescriptorHandle.IsNull())
    {
        LOG_ERROR("Failed to allocate SRV descriptor.");
        return;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = mFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    Device::GetDevice()->CreateShaderResourceView(GetResource(), &srvDesc, mSrvDescriptorHandle.GetCpuHandle());
}
