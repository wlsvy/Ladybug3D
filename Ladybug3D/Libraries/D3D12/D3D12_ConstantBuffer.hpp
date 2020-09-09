#pragma once
#include "D3D12_Resource.hpp"
#include "d3dx12.h"

namespace Ladybug3D::D3D12 {

	template<typename T>
	class ConstantBuffer : public Resource {
	public:
		ConstantBuffer(ID3D12Device* device, UINT bufferSize = 1);
		void CreateConstantBufferView(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor, UINT offset = 0);
		T* Data;
	};

	template<typename T>
	inline ConstantBuffer<T>::ConstantBuffer(ID3D12Device* device, UINT bufferSize)
	{
		CreateBuffer(device, sizeof(T) * bufferSize, D3D12_HEAP_TYPE_UPLOAD);

		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(m_Resource->Map(0, &readRange, reinterpret_cast<void**>(&Data)));
	}

	template<typename T>
	inline void ConstantBuffer<T>::CreateConstantBufferView(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor, UINT offset)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.SizeInBytes = sizeof(T);
		desc.BufferLocation = m_Resource->GetGPUVirtualAddress() + offset * desc.SizeInBytes;
		device->CreateConstantBufferView(&desc, cpuDescriptor);
	}
}