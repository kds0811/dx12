#pragma once
#include "GraphicCommonHeaders.h"

class Pso;
class CommandAllocator;
class CommandManager;

/// \brief Represents a Direct3D 12 command list and provides methods for recording and managing GPU commands.
///
/// This class encapsulates a Direct3D 12 command list and its associated command allocator. It provides methods for recording GPU commands (e.g., drawing, setting pipeline states)
/// and ensures proper state management (e.g., open/closed state).
///
/// Key responsibilities:
/// - Recording GPU commands (e.g., `DrawInstanced`, `ResourceBarrier`, etc.).
/// - Managing the lifecycle of the command list (e.g., resetting, closing).
/// - Synchronizing with the GPU using fences (`mFenceValue`).
/// - Ensuring thread safety and valid state transitions (e.g., checking if the command list is closed before adding commands).
class CommandList
{
    friend CommandQueue;
    friend CommandManager;

    std::unique_ptr<CommandAllocator> mCommandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    bool bIsClosed = false;

    int mID = -1;

public:
    CommandList(ID3D12Device* device, UINT id);
    ~CommandList();
    CommandList(const CommandList&) = delete;
    CommandList& operator=(const CommandList&) = delete;

    CommandList(CommandList&& other) noexcept;
    CommandList& operator=(CommandList&& other) noexcept;

    [[nodiscard]] inline bool IsClosed() const noexcept { return bIsClosed; }
    [[nodiscard]] inline bool IsInitialized() const noexcept { return mCommandAllocator && mCommandList; }
    UINT64 GetFenceValue() const noexcept;


    void ResourceBarrier(UINT numBarriers, const D3D12_RESOURCE_BARRIER* barriers);
    void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, const FLOAT colorRGBA[4], UINT numRects, const D3D12_RECT* rects);
    void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation);
    void SetPipelineState(ID3D12PipelineState* pipelineState);
    void SetGraphicsRootSignature(ID3D12RootSignature* rootSignature);
    void IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology);
    void IASetVertexBuffers(UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* views);
    void OMSetRenderTargets(UINT numRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* renderTargetDescriptors, BOOL singleDescriptorHandle,
        const D3D12_CPU_DESCRIPTOR_HANDLE* depthStencilDescriptor);

private:
    void Initialize(ID3D12Device* device, UINT id);

    /// \brief Checks if the command list is in a valid state for adding commands.
    /// \return True if the command list is valid, false otherwise.
    [[nodiscard]] bool IsValidState();

    /// \brief Checks if the command list is in a valid state for resetting.
    /// \return True if the command list can be reset, false otherwise.
    [[nodiscard]] bool IsValidStateForReset();

    [[nodiscard]] bool ResetCommandList(ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipelineState);

    [[nodiscard]] inline ID3D12GraphicsCommandList* GetCommandList() const noexcept { return mCommandList.Get(); }

    [[nodiscard]] bool ResetAllocatorAndCommandList(Pso* pso, UINT64 queueLastCompletedFenceValue);

    [[nodiscard]] bool Close();

    void SetFenceValue(UINT64 value) noexcept;
};
