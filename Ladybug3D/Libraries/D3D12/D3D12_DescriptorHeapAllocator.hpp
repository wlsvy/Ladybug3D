#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include "d3dx12.h"

namespace Ladybug3D::D3D12 {
	class DescriptorHeapAllocator {
	public:
		DescriptorHeapAllocator(
			ID3D12Device* device,
			D3D12_DESCRIPTOR_HEAP_TYPE heapType,
			D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag,
			uint32_t numDescriptorsPerHeap = 1024);
		~DescriptorHeapAllocator();

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(int offset = 0) const;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(int offset = 0) const;
		auto GetDescriptorHeap() { return m_DescriptorHeap.Get(); }
		auto GetDescriptorCapacity() const { return m_NumDescriptors; }
		auto GetIncrementSize() const { return m_HandleIncrementSize; }
		auto GetHeapType() const { return m_HeapType; }
	private:

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE m_HeapType;
		uint32_t m_NumDescriptors;
		uint32_t m_HandleIncrementSize;
		CD3DX12_GPU_DESCRIPTOR_HANDLE m_GPUDescriptorHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_CPUDescriptorHandle;
	};
}