#pragma once
#include "FrameResource.h"

class FrameResourceManager
{
    std::vector<std::unique_ptr<FrameResource>> mFrameResources;
    FrameResource* mCurrFrameResource = nullptr;
    int mCurrFrameResourceIndex = 0;

    static constexpr size_t mPassConstantCout = 2;

public:
    FrameResourceManager();

private:
    void BuildFrameResources();
};