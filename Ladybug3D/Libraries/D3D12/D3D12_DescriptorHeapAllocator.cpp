#include "D3D12_DescriptorHeapAllocator.hpp"
#include "D3D12_Util.hpp"


namespace Ladybug3D::D3D12 {

	DescriptorHeapAllocator::DescriptorHeapAllocator(
		ID3D12Device* device,
		D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag,
		uint32_t numDescriptorsPerHeap)
		:
		m_HeapType(heapType),
		m_NumDescriptors(numDescriptorsPerHeap)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = m_HeapType;
		desc.NumDescriptors = m_NumDescriptors;
		desc.Flags = heapFlag;
		ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_DescriptorHeap.GetAddressOf())));

		m_CPUDescriptorHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_GPUDescriptorHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		m_HandleIncrementSize = device->GetDescriptorHandleIncrementSize(m_HeapType);
	}

	DescriptorHeapAllocator::~DescriptorHeapAllocator()
	{
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeapAllocator::GetCpuHandle(int offset) const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CPUDescriptorHandle, offset, m_HandleIncrementSize);
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeapAllocator::GetGpuHandle(int offset) const
	{
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_GPUDescriptorHandle, offset, m_HandleIncrementSize);
	}


}

