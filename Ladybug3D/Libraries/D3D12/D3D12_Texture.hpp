#pragma once
#include "D3D12_Resource.hpp"
#include <d3d12.h>
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
		using Vertex = DirectX::VertexPositionNormalTexture;

		VertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<Vertex>& vertices);
		~VertexBuffer();

	private:
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	};

	class IndesBuffer : public Resource {
	public:
		IndesBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<UINT>& indices);
		~IndesBuffer();

	private:
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
		UINT m_NumIndices;
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