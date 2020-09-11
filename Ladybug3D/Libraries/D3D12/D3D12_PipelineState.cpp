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
		D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelinestateDesc, 
		D3D12_VERSIONED_ROOT_SIGNATURE_DESC* rootSignatureDesc)
		: rs(pipelinestateDesc->pRootSignature)
	{
		ThrowIfFailed(device->CreateGraphicsPipelineState(pipelinestateDesc, IID_PPV_ARGS(&m_pipelineState)),
			"Failed To Create Pipeline State Object");
	}

	PipelineState::~PipelineState()
	{
	}

	void PipelineState::InitializeRootSignature()
	{
	}
}