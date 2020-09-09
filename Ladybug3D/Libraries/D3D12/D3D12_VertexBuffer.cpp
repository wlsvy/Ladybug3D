#include "D3D12_VertexBuffer.hpp"
#include "D3D12_Util.hpp"
#include "D3D12_VertexType.hpp"
#include "D3D12_CommandList.hpp"
#include "d3dx12.h"

using namespace Microsoft::WRL;

namespace Ladybug3D::D3D12 {

	void VertexBuffer::CreateVertexBuffer(ID3D12Device* device, GraphicsCommandList* cmdList, UINT vertexCount, UINT vertexTypeSize, const void* data)
	{
		UINT vertexDataSize = vertexCount * vertexTypeSize;

		ComPtr<ID3D12Resource> bufferUploadHeap;

		CreateBuffer(device, vertexDataSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);
		CreateBuffer(device, vertexDataSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, &bufferUploadHeap);
		
		cmdList->TrackObject(bufferUploadHeap);

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = data;
		vertexData.RowPitch = vertexDataSize;
		vertexData.SlicePitch = vertexData.RowPitch;

		UpdateSubresources<1>(cmdList->GetCommandList(), m_Resource.Get(), bufferUploadHeap.Get(), 0, 0, 1, &vertexData);
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		m_VertexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = vertexTypeSize;
		m_VertexBufferView.SizeInBytes = vertexDataSize;
	}
}