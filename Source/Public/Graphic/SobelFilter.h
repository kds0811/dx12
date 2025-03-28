#pragma once
#include "D3D12Utils.h"
#include "GpuResource.h"
#include <memory>

class SobelFilter
{
    ID3D12Device* mDevice = nullptr;

    UINT mWidth = 0;
    UINT mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuUav;

    CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuUav;

    // Microsoft::WRL::ComPtr<ID3D12Resource> mOutput = nullptr;
    std::unique_ptr<GpuResource> mOutput = nullptr;

public:
	SobelFilter(ID3D12Device* device,
		UINT width, UINT height,
		DXGI_FORMAT format);
		
	SobelFilter(const SobelFilter& rhs)=delete;
	SobelFilter& operator=(const SobelFilter& rhs)=delete;
	~SobelFilter()=default;

	CD3DX12_GPU_DESCRIPTOR_HANDLE OutputSrv();

	UINT DescriptorCount()const;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor, 
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
		UINT descriptorSize);

	void OnResize(UINT newWidth, UINT newHeight);

	void Execute(
		ID3D12GraphicsCommandList* cmdList, 
		ID3D12RootSignature* rootSig,
		ID3D12PipelineState* pso,
		CD3DX12_GPU_DESCRIPTOR_HANDLE input);

private:
	void BuildDescriptors();
};

 