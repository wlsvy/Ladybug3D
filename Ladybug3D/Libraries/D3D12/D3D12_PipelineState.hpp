#pragma once
#include "D3D12_Define.hpp"

namespace Ladybug3D::D3D12 {

	class PipelineState {
	public:
		PipelineState(ID3D12Device* device, D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelinestateDesc, D3D12_VERSIONED_ROOT_SIGNATURE_DESC* rootSignatureDesc);
		~PipelineState();

		void InitializeRootSignature();

		auto GetPipelinsState() { return m_pipelineState.Get(); }
		auto GetRootSignature() { return m_rootSignature.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		ID3D12RootSignature* rs;

	};
}