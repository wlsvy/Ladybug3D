#include "D3D12_VertexBuffer.hpp"
#include "D3D12_Util.hpp"
#include "D3D12_VertexType.hpp"

namespace Ladybug3D::D3D12 {
    VertexBuffer::VertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<Vertex3D>& vertices)
    {
        UINT vertexDataSize = vertices.size() * sizeof(Vertex3D);

        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexDataSize),
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(m_Resource.GetAddressOf())));

        NAME_D3D12_OBJECT(m_Resource);

        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexDataSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(m_BufferUploadHeap.GetAddressOf())));

        D3D12_SUBRESOURCE_DATA vertexData = {};
        vertexData.pData = vertices.data();
        vertexData.RowPitch = vertexDataSize;
        vertexData.SlicePitch = vertexData.RowPitch;

        UpdateSubresources<1>(cmdList, m_Resource.Get(), m_BufferUploadHeap.Get(), 0, 0, 1, &vertexData);
        cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

        // Initialize the vertex buffer view.
        m_VertexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
        m_VertexBufferView.StrideInBytes = sizeof(Vertex3D);
        m_VertexBufferView.SizeInBytes = vertexDataSize;
    }
    VertexBuffer::~VertexBuffer()
    {
    }
}