#include "D3D12_PipelineState.hpp"
#include <d3d12.h>
#include "d3dx12.h""
#include <d3dcompiler.h>
#include "D3D12_Util.hpp"

using namespace std;
using namespace DirectX;
using namespace Ladybug3D::D3D12;
using namespace Microsoft::WRL;

namespace Ladybug3D::D3D12 {

	PipelineState::PipelineState(
		ID3D12Device* device, 
		D3D12_GRAPHICS_PIPELINE_STATE_DESC* psoDesc)
		: rs(psoDesc->pRootSignature)
		, m_PrimitiveTopologyType(psoDesc->PrimitiveTopologyType)
	{
		ThrowIfFailed(device->CreateGraphicsPipelineState(psoDesc, IID_PPV_ARGS(&m_pipelineState)),
			"Failed To Create Pipeline State Object");
	}

	PipelineState::~PipelineState()
	{
	}

	void PipelineState::InitializeRootSignature()
	{
	}
	D3D_PRIMITIVE_TOPOLOGY PipelineState::GetPrimitiveTopology()
	{
		switch (m_PrimitiveTopologyType) {
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH:
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED: return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

		}
	}
}