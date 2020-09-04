#include "D3D12_Texture.hpp"
#include "D3D12_Util.hpp"
#include <Direct12XTK/Include/WICTextureLoader.h>
#include <Direct12XTK/Include/DirectXHelpers.h>
#include <Direct12XTK/Include/ResourceUploadBatch.h>
#include <Direct12XTK/Include/VertexTypes.h>

using namespace DirectX;
using namespace Microsoft::WRL;

namespace Ladybug3D::D3D12 {
	Texture::Texture() 
	{
	}
	Texture::~Texture() 
	{
	}

	void Texture::InitializeWICTexture(
		const wchar_t* filePath, 
		ResourceUploadBatch& uploadBatch, 
		ID3D12Device* device)
	{
		ThrowIfFailed(CreateWICTextureFromFile(device, uploadBatch, filePath, m_Resource.GetAddressOf(), true));
		uploadBatch.Transition(m_Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void Texture::CreateShaderResourceView(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE descriptor)
	{
		DirectX::CreateShaderResourceView(device, m_Resource.Get(), descriptor);
	}
	VertexBuffer::VertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<Vertex>& vertices)
	{
        UINT vertexDataSize = vertices.size() * sizeof(Vertex);
        ComPtr<ID3D12Resource> vertexBufferUploadHeap;

        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexDataSize),
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(m_Resource.GetAddressOf())));

        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexDataSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(vertexBufferUploadHeap.GetAddressOf())));

        D3D12_SUBRESOURCE_DATA vertexData = {};
        vertexData.pData = vertices.data();
        vertexData.RowPitch = vertexDataSize;
        vertexData.SlicePitch = vertexData.RowPitch;

        UpdateSubresources<1>(cmdList, m_Resource.Get(), vertexBufferUploadHeap.Get(), 0, 0, 1, &vertexData);
        cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

        // Initialize the vertex buffer view.
        m_VertexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
        m_VertexBufferView.StrideInBytes = sizeof(Vertex);
        m_VertexBufferView.SizeInBytes = vertexDataSize;
	}
	VertexBuffer::~VertexBuffer()
	{
	}
    IndesBuffer::IndesBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<UINT>& indices)
    {
        UINT indexDataSize = indices.size() * sizeof(UINT);
        ComPtr<ID3D12Resource> indexBufferUploadHeap;

        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(indexDataSize),
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_Resource)));

        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(indexDataSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&indexBufferUploadHeap)));

        D3D12_SUBRESOURCE_DATA indexData = {};
        indexData.pData = indices.data();
        indexData.RowPitch = indexDataSize;
        indexData.SlicePitch = indexData.RowPitch;

        UpdateSubresources<1>(cmdList, m_Resource.Get(), indexBufferUploadHeap.Get(), 0, 0, 1, &indexData);
        cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

        // Describe the index buffer view.
        m_IndexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
        m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
        m_IndexBufferView.SizeInBytes = indexDataSize;

        m_NumIndices = indices.size();
    }
    IndesBuffer::~IndesBuffer()
    {
    }
}