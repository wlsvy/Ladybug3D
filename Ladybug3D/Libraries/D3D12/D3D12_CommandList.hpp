#pragma once
#include "D3D12_Define.hpp"
#include <vector>

namespace Ladybug3D::D3D12 {

	class GraphicsCommandList {
	public:
		GraphicsCommandList(ID3D12Device* device);
		~GraphicsCommandList();

		bool Begin(ID3D12PipelineState* pipelineState = nullptr);
		void Close();
		void ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float* clearColor);
		void ResourceBarrier(UINT numBarrier, const D3D12_RESOURCE_BARRIER* barrier);
		void SetRenderTarget(UINT numDesriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* rtv, const D3D12_CPU_DESCRIPTOR_HANDLE* dsv = nullptr, bool isSingleToRange = false);
		void TrackObject(const Microsoft::WRL::ComPtr<ID3D12Object>& obj);

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

		std::vector<Microsoft::WRL::ComPtr<ID3D12Object>> m_TrackedObject;
	};
}