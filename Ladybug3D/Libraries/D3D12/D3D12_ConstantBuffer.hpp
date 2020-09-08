#pragma once
#include "D3D12_Define.hpp"
#include "D3D12_Resource.hpp"
#include "d3dx12.h"

namespace Ladybug3D::D3D12 {

	template<typename T>
	class ConstantBuffer : public Resource {
	public:
		ConstantBuffer(ID3D12Device* device);
		~ConstantBuffer();
		void CreateConstantBufferView(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor);
		T* Data;
	};

	template<typename T>
	inline ConstantBuffer<T>::ConstantBuffer(ID3D12Device* device)
	{
		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeof(T)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(m_Resource.GetAddressOf())
		));

		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(m_Resource->Map(0, &readRange, reinterpret_cast<void**>(&Data)));
	}
	template<typename T>
	inline ConstantBuffer<T>::~ConstantBuffer()
	{
		Data = nullptr;
	}
	template<typename T>
	inline void ConstantBuffer<T>::CreateConstantBufferView(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = m_Resource->GetGPUVirtualAddress();
		desc.SizeInBytes = sizeof(T);
		device->CreateConstantBufferView(&desc, cpuDescriptor);
	}
}