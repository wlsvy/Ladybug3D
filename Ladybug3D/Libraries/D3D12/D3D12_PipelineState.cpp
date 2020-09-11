#include "D3D12_PipelineState.hpp"
#include <d3d12.h>
#include <d3dcompiler.h>
#include "D3D12_Util.hpp"

namespace Ladybug3D::D3D12 {

	PipelineState::PipelineState(ID3D12Device* device, D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc)
	{
		ThrowIfFailed(device->CreateGraphicsPipelineState(desc, IID_PPV_ARGS(&m_pipelineState)),
			"Failed To Create Pipeline State Object");
	}

	PipelineState::~PipelineState()
	{
	}

	void PipelineState::InitializeRootSignature()
	{
	}
}