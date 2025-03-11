#pragma once
#include "D3D12Utils.h"
#include "GpuResource.h"
#include <memory>

class BlurFilter
{
    const int MaxBlurRadius = 5;

    ID3D12Device* mDevice = nullptr;

    UINT mWidth = 0;
    UINT mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur0CpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur0CpuUav;

    CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur1CpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur1CpuUav;

    CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur0GpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur0GpuUav;

    CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur1GpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur1GpuUav;

    std::unique_ptr<GpuResource> mBlurMap0 = nullptr;
    std::unique_ptr<GpuResource> mBlurMap1 = nullptr;

public:
    ///< summary>
    /// The width and height should match the dimensions of the input texture to blur.
    /// Recreate when the screen is resized.
    ///</summary>
    BlurFilter(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format);

    BlurFilter(const BlurFilter& rhs) = delete;
    BlurFilter& operator=(const BlurFilter& rhs) = delete;
    ~BlurFilter() = default;

    ID3D12Resource* Output();

    void BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor, UINT descriptorSize);

    void OnResize(UINT newWidth, UINT newHeight);

    ///< summary>
    /// Blurs the input texture blurCount times.
    ///</summary>
    void Execute(ID3D12GraphicsCommandList* cmdList, ID3D12RootSignature* rootSig, ID3D12PipelineState* horzBlurPSO, ID3D12PipelineState* vertBlurPSO, ID3D12Resource* input,
        int blurCount, float sigmaColor, float blurWeiqhts);

private:
    std::vector<float> CalcGaussWeights(float sigma);
    void BuildDescriptors();
};
