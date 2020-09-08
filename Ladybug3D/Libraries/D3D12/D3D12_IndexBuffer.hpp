#pragma once
#include "D3D12_Define.hpp"
#include "D3D12_Resource.hpp"
#include <vector>
#include "d3dx12.h"

namespace Ladybug3D::D3D12 {
	class IndexBuffer : public Resource {
	public:
		IndexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<UINT>& indices);
		~IndexBuffer();

		const auto GetView() const { return &m_IndexBufferView; }
		const auto GetNumIndices() const { return m_NumIndices; }

	private:
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
		UINT m_NumIndices;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_BufferUploadHeap;

	};
}