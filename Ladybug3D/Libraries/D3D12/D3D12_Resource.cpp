#include "D3D12_Resource.hpp"
#include "D3D12_Util.hpp"
#include "d3dx12.h"

namespace Ladybug3D::D3D12 {

	void Resource::Reset() {
		m_Resource.Reset();
	}

	void Resource::CreateBuffer(
		ID3D12Device* device,
		UINT size, 
		D3D12_HEAP_TYPE heapType, 
		D3D12_HEAP_FLAGS heapFlag, 
		D3D12_RESOURCE_STATES intialResourceState, 
		ID3D12Resource** resource,
		const D3D12_CLEAR_VALUE* clearValue)
	{
		CreateBuffer(device, CD3DX12_RESOURCE_DESC::Buffer(size), heapType, heapFlag, intialResourceState, resource, clearValue);
	}

	void Resource::CreateBuffer(
		ID3D12Device* device, 
		D3D12_RESOURCE_DESC& resourceDesc, 
		D3D12_HEAP_TYPE heapType, 
		D3D12_HEAP_FLAGS heapFlag, 
		D3D12_RESOURCE_STATES intialResourceState, 
		ID3D12Resource** resource, 
		const D3D12_CLEAR_VALUE* clearValue)
	{
		if (resource == nullptr) {
			resource = m_Resource.ReleaseAndGetAddressOf();
		}

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(heapType),
			heapFlag,
			&resourceDesc,
			intialResourceState,
			clearValue,
			IID_PPV_ARGS(resource)));

		SetName(*resource, L"Buffer Resource");
	}
}

