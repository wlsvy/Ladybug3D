#include "D3D12_VertexBuffer.hpp"
#include "D3D12_Util.hpp"
#include "D3D12_VertexType.hpp"
#include "d3dx12.h"

namespace Ladybug3D::D3D12 {

	void VertexBuffer::CreateVertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, UINT vertexCount, UINT vertexTypeSize, const void* data)
	{
		UINT vertexDataSize = vertexCount * vertexTypeSize;

		CreateBuffer(device, vertexDataSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);
		CreateBuffer(device, vertexDataSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, &m_BufferUploadHeap);

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = data;
		vertexData.RowPitch = vertexDataSize;
		vertexData.SlicePitch = vertexData.RowPitch;

		UpdateSubresources<1>(cmdList, m_Resource.Get(), m_BufferUploadHeap.Get(), 0, 0, 1, &vertexData);
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		m_VertexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = vertexTypeSize;
		m_VertexBufferView.SizeInBytes = vertexDataSize;
	}
}