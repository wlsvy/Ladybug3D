#pragma once
#include "D3D12_Define.hpp"
#include <d3d12.h>

namespace Ladybug3D::D3D12 {
	class Resource {
	public:
		Resource() = default;
		virtual ~Resource() = default;
		Resource(const Resource&) = delete;
		Resource(Resource&&) noexcept = delete;
		Resource& operator=(const Resource&) = delete;
		Resource& operator=(Resource&&) noexcept = default;

		ID3D12Resource* operator->() { return m_Resource.Get(); }
		const ID3D12Resource* operator->() const { return m_Resource.Get(); }

		auto GetResource() { return m_Resource.Get(); }
		auto GetResourceAddress() { return m_Resource.GetAddressOf(); }

	protected:
		void CreateBuffer(
			ID3D12Device* device,
			UINT size, 
			D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT,
			D3D12_HEAP_FLAGS heapFlag = D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_STATES intialResourceState = D3D12_RESOURCE_STATE_GENERIC_READ,
			ID3D12Resource** resource = nullptr,
			const D3D12_CLEAR_VALUE* clearValue = nullptr);

		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
	};
}