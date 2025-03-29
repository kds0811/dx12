#include "CommandList.h"
#include "Pso.h"
#include "CommandAllocator.h"

CommandList::CommandList(ID3D12Device* device)
{
    assert(device);
    Initialize(device);
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

void CommandList::Close()
{
    if (IsValidState())
    {
        mCommandList->Close() >> Kds::App::Check;
        bIsClosed = true;
    }
}

bool CommandList::ResetWithOwnAlloc(Pso* pso, UINT64 queueLastCompletedFenceValue)
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

bool CommandList::ResetWithAnotherAlloc(Pso* pso, UINT64 queueLastCompletedFenceValue, CommandAllocator* commandAllocator)
{
    if (!pso)
    {
        LOG_ERROR("Pso pointer is nullptr");
        return false;
    }

    if (!IsValidStateForReset()) return false;

    if (!commandAllocator->ResetCommandAllocatorIfFenceComplited(queueLastCompletedFenceValue)) return false;

    return ResetCommandList(commandAllocator->GetCommandListAllocator(), pso->GetPso());

}

void CommandList::Initialize(ID3D12Device* device)
{
    if (mCommandAllocator && mCommandList)
    {
        LOG_WARNING("CommandList is already initialized.");
        return;
    }

    mCommandAllocator = std::make_unique<CommandAllocator>(device);

    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator->GetCommandListAllocator(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())) >>
        Kds::App::Check;
    mCommandList->SetName(L"Command List");
    LOG_MESSAGE("Command List has been created");

    mCommandList->Close();
    bIsClosed = true;
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
