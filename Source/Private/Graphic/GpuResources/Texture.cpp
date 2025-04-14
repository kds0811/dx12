#include "Texture.h"
#include "GraphicError.h"
#include "DDSTextureLoader.h"
#include "Logger.h"

Texture::Texture(ETextureType type, std::wstring name, std::wstring fileName, ID3D12GraphicsCommandList* cmdList)
	:
	mType(type),
	mFilename(std::move(fileName))
{
    LoadFromFile(device, cmdList);
}

void Texture::LoadFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) 
{
    DirectX::CreateDDSTextureFromFile12(device, cmdList, mFilename.c_str(), GetComPtr(), mUploadHeap) >> Kds::App::Check;

   
	auto desc = mResource->GetDesc();
    mFormat = desc.Format;
    mWidth = desc.Width;
    mHeight = desc.Height;
 
    
    LOG_MESSAGE("Texture ", GetName(), " is created");
}
