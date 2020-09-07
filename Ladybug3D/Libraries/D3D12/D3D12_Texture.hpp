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

	struct Vertex {
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT2 uv = DirectX::XMFLOAT2(0.0f, 0.0f);
		DirectX::XMFLOAT4 tangent = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	};
	struct Vertex2 {
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	};

	template<typename T>
	class ConstantBuffer : public Resource {
	public:
		ConstantBuffer(ID3D12Device* device);
		~ConstantBuffer();
		void CreateConstantBufferView(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor);
		T* Data;
	};


	class VertexBuffer : public Resource {
	public:
		VertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<Vertex>& vertices);
		VertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<Vertex2>& vertices);
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

	class Shader {

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