#pragma once
#include "D3D12_Resource.hpp"
#include <vector>

namespace Ladybug3D::D3D12 {

	class VertexBuffer : public Resource {
	public:

		template<typename VertexType>
		VertexBuffer(ID3D12Device* device, GraphicsCommandList* cmdList, const std::vector<VertexType>& vertices) {
			CreateVertexBuffer(device, cmdList, vertices.size(), sizeof(VertexType), vertices.data());
		}

		const auto GetView() const { return &m_VertexBufferView; }

	private:
		void CreateVertexBuffer(ID3D12Device* device, GraphicsCommandList* cmdList, UINT vertexCount, UINT vertexTypeSize, const void* data);

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	};
}