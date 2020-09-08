#include "D3D12_IndexBuffer.hpp"
#include "D3D12_Util.hpp"
#include "d3dx12.h"

namespace Ladybug3D::D3D12 {
    IndexBuffer::IndexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<UINT>& indices)
    {
        UINT indexDataSize = indices.size() * sizeof(UINT);

        CreateBuffer(device, indexDataSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST );
        CreateBuffer(device, indexDataSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, &m_BufferUploadHeap);

        D3D12_SUBRESOURCE_DATA indexData = {};
        indexData.pData = indices.data();
        indexData.RowPitch = indexDataSize;
        indexData.SlicePitch = indexData.RowPitch;

        UpdateSubresources<1>(cmdList, m_Resource.Get(), m_BufferUploadHeap.Get(), 0, 0, 1, &indexData);
        cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

        m_IndexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
        m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
        m_IndexBufferView.SizeInBytes = indexDataSize;

        m_NumIndices = indices.size();
    }
    IndexBuffer::~IndexBuffer()
    {
    }
}