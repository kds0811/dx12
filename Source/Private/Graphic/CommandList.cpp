#include "CommandList.h"
#include "Pso.h"
#include "CommandAllocator.h"
#include "CommandQueue.h"
#include "CommandManager.h"
#include "string"
#include "ResourceManager.h"

CommandList::CommandList(ID3D12Device* device, UINT id)
{
    assert(device);
    Initialize(device, id);
}

CommandList::~CommandList() = default;

CommandList::CommandList(CommandList&& other) noexcept
    : mCommandAllocator(std::move(other.mCommandAllocator)), mCommandList(std::move(other.mCommandList)), bIsClosed(other.bIsClosed)
{
    other.bIsClosed = true;
}

CommandList& CommandList::operator=(CommandList&& other) noexcept
{
    if (this != &other)
    {
        mCommandAllocator = std::move(other.mCommandAllocator);
        mCommandList = std::move(other.mCommandList);
        bIsClosed = other.bIsClosed;
        other.bIsClosed = true;
    }
    return *this;
}

bool CommandList::Close()
{
    if (!IsValidState()) return false;

    if (IsClosed()) return false;

    mCommandList->Close() >> Kds::App::Check;
    bIsClosed = true;
    return true;
}

bool CommandList::ResetAllocatorAndCommandList(Pso* pso, UINT64 queueLastCompletedFenceValue)
{
    if (!pso)
    {
        LOG_ERROR("Pso pointer is nullptr");
        return false;
    }

    if (!IsValidStateForReset()) return false;

    if (!mCommandAllocator->ResetCommandAllocatorIfFenceComplited(queueLastCompletedFenceValue)) return false;

    return ResetCommandList(mCommandAllocator->GetCommandListAllocator(), pso->GetPso());
}

UINT64 CommandList::GetFenceValue() const noexcept
{
    return mCommandAllocator->GetFenceValue();
}

void CommandList::SetFenceValue(UINT64 value) noexcept
{
    mCommandAllocator->SetFenceValue(value);
}

void CommandList::Initialize(ID3D12Device* device, UINT id)
{
    if (mCommandAllocator && mCommandList)
    {
        LOG_WARNING("CommandList is already initialized.");
        return;
    }

    mCommandAllocator = std::make_unique<CommandAllocator>(device);

    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator->GetCommandListAllocator(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())) >>
        Kds::App::Check;

    mCommandList->Close();
    bIsClosed = true;
    mID = static_cast<int>(id);
    std::wstring name = L"Command List ID: " + std::to_wstring(mID);
    mCommandList->SetName(name.c_str());
    LOG_MESSAGE("Command List has been created");
}

void CommandList::ResourceBarrier(UINT numBarriers, const D3D12_RESOURCE_BARRIER* barriers)
{
    if (IsValidState())
    {
        mCommandList->ResourceBarrier(numBarriers, barriers);
    }
}

void CommandList::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, const FLOAT colorRGBA[4], UINT numRects, const D3D12_RECT* rects)
{
    if (IsValidState())
    {
        mCommandList->ClearRenderTargetView(renderTargetView, colorRGBA, numRects, rects);
    }
}

void CommandList::DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation)
{
    if (IsValidState())
    {
        mCommandList->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
    }
}

void CommandList::SetPipelineState(ID3D12PipelineState* pipelineState)
{
    if (IsValidState())
    {
        mCommandList->SetPipelineState(pipelineState);
    }
}

void CommandList::SetGraphicsRootSignature(ID3D12RootSignature* rootSignature)
{
    if (IsValidState())
    {
        mCommandList->SetGraphicsRootSignature(rootSignature);
    }
}

void CommandList::IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology)
{
    if (IsValidState())
    {
        mCommandList->IASetPrimitiveTopology(primitiveTopology);
    }
}

void CommandList::IASetVertexBuffers(UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* views)
{
    if (IsValidState())
    {
        mCommandList->IASetVertexBuffers(startSlot, numViews, views);
    }
}

void CommandList::OMSetRenderTargets(UINT numRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* renderTargetDescriptors, BOOL singleDescriptorHandle,
    const D3D12_CPU_DESCRIPTOR_HANDLE* depthStencilDescriptor)
{
    if (IsValidState())
    {
        mCommandList->OMSetRenderTargets(numRenderTargetDescriptors, renderTargetDescriptors, singleDescriptorHandle, depthStencilDescriptor);
    }
}

void CommandList::ClearDepthStencilView(
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView, D3D12_CLEAR_FLAGS clearFlags, FLOAT depth, UINT8 stencil, UINT numRects, const D3D12_RECT* pRects)
{
    mCommandList->ClearDepthStencilView(depthStencilView, clearFlags, depth, stencil, numRects, pRects);
}

bool CommandList::IsValidState()
{
    assert(mCommandList && !bIsClosed);

    if (!mCommandList)
    {
        LOG_ERROR("CommandList: Attempt to access a nullptr command list");
        return false;
    }

    if (bIsClosed)
    {
        LOG_ERROR("CommandList: Attempt to add a command to a closed command list");
        return false;
    }

    return true;
}

bool CommandList::IsValidStateForReset()
{
    assert(mCommandList && bIsClosed);

    if (!mCommandList)
    {
        LOG_ERROR("CommandList: Attempt to reset a nullptr command list");
        return false;
    }

    if (!bIsClosed)
    {
        LOG_ERROR("CommandList: Attempting to reset an open command list");
        return false;
    }

    return true;
}

bool CommandList::ResetCommandList(ID3D12CommandAllocator* allocator, ID3D12PipelineState* pipelineState)
{
    if (!allocator || !pipelineState)
    {
        LOG_ERROR("Invalid allocator or pipeline state");
        return false;
    }

    mCommandList->Reset(allocator, pipelineState) >> Kds::App::Check;
    bIsClosed = false;
    return true;
}
