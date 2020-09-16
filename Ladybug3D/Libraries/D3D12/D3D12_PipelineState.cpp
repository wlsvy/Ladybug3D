#include "D3D12_PipelineState.hpp"
#include <d3d12.h>
#include "d3dx12.h"
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
		: m_RootSignature(psoDesc->pRootSignature)
	{
		switch (psoDesc->PrimitiveTopologyType) {
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT: m_PrimitiveTopologyType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE: m_PrimitiveTopologyType = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE: m_PrimitiveTopologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;

		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH:
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED: m_PrimitiveTopologyType = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED; break;
		}

		ThrowIfFailed(device->CreateGraphicsPipelineState(psoDesc, IID_PPV_ARGS(&m_PipelineState)),
			"Failed To Create Pipeline State Object");
	}

	PipelineState::~PipelineState()
	{
	}
}