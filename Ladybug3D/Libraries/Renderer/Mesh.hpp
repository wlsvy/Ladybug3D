#pragma once
#include <memory>
#include <DirectXMath.h>
#include <D3D12/D3D12_VertexBuffer.hpp>
#include <D3D12/D3D12_IndexBuffer.hpp>

namespace Ladybug3D {

	class Mesh {
	public:
		Mesh(std::shared_ptr<D3D12::VertexBuffer>& vertexBuffer, std::shared_ptr<D3D12::IndexBuffer>& indexBuffer, const DirectX::XMMATRIX& worldMatrix)
			: m_VertexBuffer(vertexBuffer)
			, m_IndexBuffer(indexBuffer) 
			, m_WorldMatrix(worldMatrix)
		{}

		auto GetVertexBuffer() const { return m_VertexBuffer.get(); }
		auto GetVertexBufferView() const { return m_VertexBuffer.get()->GetView(); }
		auto GetIndexBuffer() const { return m_IndexBuffer.get(); }
		auto GetIndexBufferView() const  { return m_IndexBuffer.get()->GetView(); }
		auto GetWorldMatrix() const { return m_WorldMatrix; }
			
	private:
		std::shared_ptr<D3D12::VertexBuffer> m_VertexBuffer;
		std::shared_ptr<D3D12::IndexBuffer> m_IndexBuffer;
		DirectX::XMMATRIX m_WorldMatrix;
	};

}