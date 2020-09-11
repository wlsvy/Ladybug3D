#pragma once
#include "D3D12_Define.hpp"

namespace Ladybug3D::D3D12 {

	class PipelineState {
	public:
		PipelineState(ID3D12Device* device, D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelinestateDesc);
		~PipelineState();

		void InitializeRootSignature();

		auto GetPipelinsState() { return m_pipelineState.Get(); }
		auto GetRootSignature() { return rs; }
		D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology();

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		ID3D12RootSignature* rs;
		UINT m_PrimitiveTopologyType;
	};
}