#pragma once
#include "FrameResource.h"
#include "Logger.h"

class FrameResourceManager
{
    std::vector<std::unique_ptr<FrameResource>> mFrameResources;
    FrameResource* mCurrFrameResource = nullptr;
    int mCurrFrameResourceIndex = 0;
    static constexpr size_t mPassConstantCount = 2;

    // Cache number of frame resources
    size_t mNumFrameResources = 0;

public:
    FrameResourceManager();
    ~FrameResourceManager();

    void CycleToNextFrameResource();
    [[nodiscard]] inline UINT64 GetCurrentFenceValue()
    {
        assert(mCurrFrameResource);
        if (!mCurrFrameResource)
        {
            LOG_ERROR("Cuurent Frame Resource pointer is nullptr");
        }
        return mCurrFrameResource->GetFenceValue();
    }

    [[nodiscard]] inline UploadBuffer<PassConstants>* GetCurrentPassConstants()
    {
        assert(mCurrFrameResource);
        if (!mCurrFrameResource)
        {
            LOG_ERROR("Cuurent Frame Resource pointer is nullptr");
        }
        return mCurrFrameResource->GetPassConstants();
    }

    [[nodiscard]] inline UploadBuffer<MaterialConstants>* GetCurrentMaterialConstants()
    {
        assert(mCurrFrameResource);
        if (!mCurrFrameResource)
        {
            LOG_ERROR("Cuurent Frame Resource pointer is nullptr");
        }
        return mCurrFrameResource->GetMaterialConstants();
    }

    [[nodiscard]] inline UploadBuffer<ObjectConstants>* GetCurrentObjectConstants()
    {
        assert(mCurrFrameResource);
        if (!mCurrFrameResource)
        {
            LOG_ERROR("Cuurent Frame Resource pointer is nullptr");
        }
        return mCurrFrameResource->GetObjectConstants();
    }

    [[nodiscard]] inline ID3D12CommandAllocator* GetCurrentCommandListAllocator()
    {
        assert(mCurrFrameResource);
        if (!mCurrFrameResource)
        {
            LOG_ERROR("Cuurent Frame Resource pointer is nullptr");
        }
        return mCurrFrameResource->GetCommandListAllocator();
    }

    inline void ResetCurrentCommandListallocator() { mCurrFrameResource->GetCommandListAllocator()->Reset(); }

    inline void SetFenceValueToCurrentFrameResource(UINT64 value) { mCurrFrameResource->SetFenceValue(value); }

private:
    void BuildFrameResources();
};
