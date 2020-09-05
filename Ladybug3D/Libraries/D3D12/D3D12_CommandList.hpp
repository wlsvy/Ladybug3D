#pragma once
#include <wrl/client.h>

struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct ID3D12CommandAllocator;
struct ID3D12Fence;
struct ID3D12PipelineState;
struct ID3D12DescriptorHeap;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct D3D12_RESOURCE_BARRIER;

namespace Ladybug3D::D3D12 {

	class CommandList {
	public:
		CommandList();
		~CommandList();



	private:


	};

	class GraphicsCommandList {
	public:
		GraphicsCommandList(ID3D12Device* device);
		~GraphicsCommandList();

		bool Begin(ID3D12PipelineState* pipelineState = nullptr);
		void Close();
		void ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float* clearColor);
		void ResourceBarrier(UINT numBarrier, const D3D12_RESOURCE_BARRIER* barrier);
		void SetVertexBuffer();
		void SetIndexBuffer();
		void SetTexture();
		void SetRenderTarget(UINT numDesriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* rtv, const D3D12_CPU_DESCRIPTOR_HANDLE* dsv = nullptr, bool isSingleToRange = false);
		void SetConstantBuffer();

		void DrawIndexed();
		void DrawMesh();

		auto GetCommandList() { return m_CommandList.Get(); }
		auto GetFence() { return m_Fence.Get(); }
		auto GetFenceEvent() { return m_FenceEvent; }
		auto GetFenceValue() const { return m_FenceValue; }
		auto GetAndIncreaseFenceValue() { return m_FenceValue++; }

	private:

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;

		UINT64 m_FenceValue = 1;
		HANDLE m_FenceEvent;
	};
}