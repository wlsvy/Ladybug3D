#pragma once
#include "D3D12_Define.hpp"
#include "D3D12_Resource.hpp"
#include <d3d12.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include <vector>

namespace DirectX {
	class ResourceUploadBatch;
	class VertexPositionNormalTexture;
}

namespace Ladybug3D::D3D12 {
	class Texture : public Resource {
	public:
		Texture();
		~Texture();

		void InitializeWICTexture(
			const wchar_t* filePath, 
			DirectX::ResourceUploadBatch& uploadBatch, 
			ID3D12Device* device);
		void CreateShaderResourceView(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE descriptor);

	private:
	};

	

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