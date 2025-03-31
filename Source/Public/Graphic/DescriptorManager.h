#pragma once
#include "GraphicCommonHeaders.h"

class DescriptorManager
{
    // Decriptor Sizes
    UINT mRtvDescriptorSize = 0u;
    UINT mDsvDescriptorSize = 0u;
    UINT mCbvSrvUavDescriptorSize = 0u;

public:
    DescriptorManager(ID3D12Device* device);

private:
    void Initialize(ID3D12Device* device);
};
 