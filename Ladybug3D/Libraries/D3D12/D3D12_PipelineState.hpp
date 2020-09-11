#pragma once
#include "D3D12_Define.hpp"

namespace Ladybug3D::D3D12 {

	class PipelineState {
	public:
		PipelineState(ID3D12Device* device, D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelinestateDesc);
		~PipelineState();

		auto GetPipelinsState() { return m_PipelineState.Get(); }
		auto GetRootSignature() { return m_RootSignature; }
		D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return m_PrimitiveTopologyType;}

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
		ID3D12RootSignature * m_RootSignature;
		D3D_PRIMITIVE_TOPOLOGY m_PrimitiveTopologyType;
	};
}