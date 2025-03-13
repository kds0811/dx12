#include "Texture.h"
#include "GraphicError.h"
#include "DDSTextureLoader.h"


Texture::Texture(ETexture2DType type, std::string name, std::wstring fileName)
	:
	mType(type),
	mName(std::move(name)),
	mFilename(std::move(fileName))
{

}

void Texture::LoadFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) 
{
    DirectX::CreateDDSTextureFromFile12(device, cmdList, mFilename.c_str(), mResource->GetComPtrResource(), mUploadHeap->GetComPtrResource()) >> Kds::App::Check;
}
