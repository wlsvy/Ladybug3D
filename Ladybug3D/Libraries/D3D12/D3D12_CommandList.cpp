#include "D3D12_CommandList.hpp"
#include <d3d12.h>
#include "d3dx12.h"
#include "D3D12_Util.hpp"
#include "D3D12_PipelineState.hpp"

namespace Ladybug3D::D3D12 {

	GraphicsCommandList::GraphicsCommandList(ID3D12Device* device)
	{
		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));
		ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)));
		ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

		ThrowIfFailed(m_CommandList->Close());

		m_FenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
	}
	GraphicsCommandList::~GraphicsCommandList()
	{
	}

	bool GraphicsCommandList::Reset(ID3D12PipelineState* pipelineState)
	{
		ThrowIfFailed(m_CommandAllocator->Reset());
		ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), pipelineState));
		m_TrackedObject.clear();

		return true;
	}

	void GraphicsCommandList::Close()
	{
		ThrowIfFailed(m_CommandList->Close());
	}

	void GraphicsCommandList::ResourceBarrier(UINT numBarrier, const D3D12_RESOURCE_BARRIER* barrier)
	{
		m_CommandList->ResourceBarrier(numBarrier, barrier);
	}

	void GraphicsCommandList::ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float* clearColor)
	{
		m_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	}

	void GraphicsCommandList::SetPipelineState(PipelineState* pso) {
		m_CommandList->SetPipelineState(pso->GetPipelinsState());
		m_CommandList->SetGraphicsRootSignature(pso->GetRootSignature());
		m_CommandList->IASetPrimitiveTopology(pso->GetPrimitiveTopology());
	}

	void GraphicsCommandList::SetRenderTarget(
		UINT numDesriptors, 
		const D3D12_CPU_DESCRIPTOR_HANDLE* rtv, 
		const D3D12_CPU_DESCRIPTOR_HANDLE* dsv, 
		bool isSingleToRange)
	{
		m_CommandList->OMSetRenderTargets(numDesriptors, rtv, isSingleToRange, dsv);
	}
	void GraphicsCommandList::TrackObject(const Microsoft::WRL::ComPtr<ID3D12Object>& obj)
	{
		m_TrackedObject.push_back(obj);
	}

	void GraphicsCommandList::DrawScreenQuad() {
		static D3D12_VERTEX_BUFFER_VIEW screenVertexBufferView = { 0, 0, 0 };
		static D3D12_INDEX_BUFFER_VIEW screenIndexBufferView = { 0, 0, DXGI_FORMAT_R32_UINT };

		m_CommandList->IASetVertexBuffers(0, 0, &screenVertexBufferView);
		m_CommandList->IASetIndexBuffer(&screenIndexBufferView);
		m_CommandList->DrawInstanced(4, 1, 0, 0);
	}
}