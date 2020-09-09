#include "D3D12_IndexBuffer.hpp"
#include "D3D12_CommandList.hpp"
#include "D3D12_Util.hpp"
#include "d3dx12.h"

using namespace Microsoft::WRL;

namespace Ladybug3D::D3D12 {
    IndexBuffer::IndexBuffer(ID3D12Device* device, GraphicsCommandList* cmdList, const std::vector<UINT>& indices)
    {
        UINT indexDataSize = indices.size() * sizeof(UINT);

        ComPtr<ID3D12Resource> bufferUploadHeap;

        CreateBuffer(device, indexDataSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST );
        CreateBuffer(device, indexDataSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, &bufferUploadHeap);

        cmdList->TrackObject(bufferUploadHeap);

        D3D12_SUBRESOURCE_DATA indexData = {};
        indexData.pData = indices.data();
        indexData.RowPitch = indexDataSize;
        indexData.SlicePitch = indexData.RowPitch;

        UpdateSubresources<1>(cmdList->GetCommandList(), m_Resource.Get(), bufferUploadHeap.Get(), 0, 0, 1, &indexData);
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