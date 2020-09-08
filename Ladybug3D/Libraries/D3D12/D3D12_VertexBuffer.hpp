#pragma once
#include "D3D12_Define.hpp"
#include "D3D12_Resource.hpp"
#include <vector>
#include "d3dx12.h"

namespace Ladybug3D::D3D12 {
	struct Vertex3D;
	class VertexBuffer : public Resource {
	public:
		VertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<Vertex3D>& vertices);
		~VertexBuffer();

		const auto GetView() const { return &m_VertexBufferView; }

	private:
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_BufferUploadHeap;
	};
}