#pragma once
#include "D3D12_Resource.hpp"
#include <vector>

namespace Ladybug3D::D3D12 {
	class IndexBuffer : public Resource {
	public:
		IndexBuffer(ID3D12Device* device, GraphicsCommandList* cmdList, const std::vector<UINT>& indices);
		~IndexBuffer();

		const auto GetView() const { return &m_IndexBufferView; }
		const auto GetNumIndices() const { return m_NumIndices; }

	private:
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
		UINT m_NumIndices;
	};
}