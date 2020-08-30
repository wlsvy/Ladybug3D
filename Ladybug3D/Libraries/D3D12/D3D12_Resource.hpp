#pragma once
#include "D3D12_Define.hpp"

namespace Ladybug3D::D3D12 {
	class Resource {
	public:
		Resource();
		~Resource();

		auto GetResource() { return m_Resource.Get(); }

	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
	};
}