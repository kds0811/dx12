#include "BlurFilter.h"
#include <DirectXMath.h>

BlurFilter::BlurFilter(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
    mDevice = device;

    mWidth = width;
    mHeight = height;
    mFormat = format;

    mBlurMap0 = std::make_unique<GpuResource>(mDevice, mWidth, mHeight, L"BlurMap0", D3D12_RESOURCE_STATE_COPY_SOURCE, mFormat, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    mBlurMap1 = std::make_unique<GpuResource>(mDevice, mWidth, mHeight, L"BlurMap1", D3D12_RESOURCE_STATE_UNORDERED_ACCESS, mFormat, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
}

ID3D12Resource* BlurFilter::Output()
{
    return mBlurMap0->GetResource();
}

void BlurFilter::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor, UINT descriptorSize)
{
    // Save references to the descriptors.
    mBlur0CpuSrv = hCpuDescriptor;
    mBlur0CpuUav = hCpuDescriptor.Offset(1, descriptorSize);
    mBlur1CpuSrv = hCpuDescriptor.Offset(1, descriptorSize);
    mBlur1CpuUav = hCpuDescriptor.Offset(1, descriptorSize);

    mBlur0GpuSrv = hGpuDescriptor;
    mBlur0GpuUav = hGpuDescriptor.Offset(1, descriptorSize);
    mBlur1GpuSrv = hGpuDescriptor.Offset(1, descriptorSize);
    mBlur1GpuUav = hGpuDescriptor.Offset(1, descriptorSize);

    BuildDescriptors();
}

void BlurFilter::OnResize(UINT newWidth, UINT newHeight)
{
    if ((mWidth != newWidth) || (mHeight != newHeight))
    {
        mWidth = newWidth;
        mHeight = newHeight;

        mBlurMap0->OnResize(mWidth, mHeight);
        mBlurMap1->OnResize(mWidth, mHeight);

        // New resource, so we need new descriptors to that resource.
        BuildDescriptors();
    }
}

void BlurFilter::Execute(ID3D12GraphicsCommandList* cmdList, ID3D12RootSignature* rootSig, ID3D12PipelineState* horzBlurPSO, ID3D12PipelineState* vertBlurPSO,
    ID3D12Resource* input, int blurCount, float sigmaColor, float blurWeiqhts)
{
    auto weights = CalcGaussWeights(blurWeiqhts);
    int blurRadius = (int)weights.size() / 2;
    float SigmaColor = sigmaColor;
    float padding[3] = {1.0f, 1.0f, 1.0f};

    cmdList->SetComputeRootSignature(rootSig);

    cmdList->SetComputeRoot32BitConstants(0, 1, &blurRadius, 0);
    cmdList->SetComputeRoot32BitConstants(0, (UINT)weights.size(), weights.data(), 1);
    cmdList->SetComputeRoot32BitConstants(0, 1, &SigmaColor, (UINT)weights.size() + 1);
    cmdList->SetComputeRoot32BitConstants(0, 3, &padding, (UINT)weights.size() + 2);

    auto ResBarRTtoCSINPUT = CD3DX12_RESOURCE_BARRIER::Transition(input, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE);
    cmdList->ResourceBarrier(1, &ResBarRTtoCSINPUT);

    mBlurMap0->ChangeState(cmdList, D3D12_RESOURCE_STATE_COPY_DEST);

    // Copy the input (back-buffer in this example) to BlurMap0.
    cmdList->CopyResource(mBlurMap0->GetResource(), input);

    mBlurMap0->ChangeState(cmdList, D3D12_RESOURCE_STATE_GENERIC_READ);

    for (int i = 0; i < blurCount; ++i)
    {
        //
        // Horizontal Blur pass.
        //
        cmdList->SetPipelineState(horzBlurPSO);
        cmdList->SetComputeRootDescriptorTable(1, mBlur0GpuSrv);
        cmdList->SetComputeRootDescriptorTable(2, mBlur1GpuUav);

        // How many groups do we need to dispatch to cover a row of pixels, where each
        // group covers 256 pixels (the 256 is defined in the ComputeShader).
        UINT numGroupsX = (UINT)ceilf(mWidth / 256.0f);
        cmdList->Dispatch(numGroupsX, mHeight, 1);

        mBlurMap0->ChangeState(cmdList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        mBlurMap1->ChangeState(cmdList, D3D12_RESOURCE_STATE_GENERIC_READ);

        //
        // Vertical Blur pass.
        //
        cmdList->SetPipelineState(vertBlurPSO);
        cmdList->SetComputeRootDescriptorTable(1, mBlur1GpuSrv);
        cmdList->SetComputeRootDescriptorTable(2, mBlur0GpuUav);

        // How many groups do we need to dispatch to cover a column of pixels, where each
        // group covers 256 pixels  (the 256 is defined in the ComputeShader).
        UINT numGroupsY = (UINT)ceilf(mHeight / 256.0f);
        cmdList->Dispatch(mWidth, numGroupsY, 1);

        mBlurMap0->ChangeState(cmdList, D3D12_RESOURCE_STATE_GENERIC_READ);
        mBlurMap1->ChangeState(cmdList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    }

    mBlurMap0->ChangeState(cmdList, D3D12_RESOURCE_STATE_COPY_SOURCE);

}

std::vector<float> BlurFilter::CalcGaussWeights(float sigma)
{
    float twoSigma2 = 2.0f * sigma * sigma;

    // Estimate the blur radius based on sigma since sigma controls the "width" of the bell curve.
    // For example, for sigma = 3, the width of the bell curve is
    int blurRadius = (int)ceil(2.0f * sigma);

    assert(blurRadius <= MaxBlurRadius);

    std::vector<float> weights;
    weights.resize(2 * blurRadius + 1);

    float weightSum = 0.0f;

    for (int i = -blurRadius; i <= blurRadius; ++i)
    {
        float x = (float)i;

        weights[i + blurRadius] = expf(-x * x / twoSigma2);

        weightSum += weights[i + blurRadius];
    }

    // Divide by the sum so all the weights add up to 1.0.
    for (int i = 0; i < weights.size(); ++i)
    {
        weights[i] /= weightSum;
    }

    return weights;
}

void BlurFilter::BuildDescriptors()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = mFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

    uavDesc.Format = mFormat;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;

    mDevice->CreateShaderResourceView(mBlurMap0->GetResource(), &srvDesc, mBlur0CpuSrv);
    mDevice->CreateUnorderedAccessView(mBlurMap0->GetResource(), nullptr, &uavDesc, mBlur0CpuUav);
    mDevice->CreateShaderResourceView(mBlurMap1->GetResource(), &srvDesc, mBlur1CpuSrv);
    mDevice->CreateUnorderedAccessView(mBlurMap1->GetResource(), nullptr, &uavDesc, mBlur1CpuUav);
}
