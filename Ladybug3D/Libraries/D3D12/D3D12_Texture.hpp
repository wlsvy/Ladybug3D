#pragma once
#include "D3D12_Resource.hpp"
#include <d3d12.h>

namespace DirectX {
	class ResourceUploadBatch;
}

namespace Ladybug3D::D3D12 {
	class Texture : public Resource {
	public:
		Texture();
		~Texture();

		void InitializeWICTexture(
			const wchar_t* filePath, 
			DirectX::ResourceUploadBatch& uploadBatch, 
			ID3D12Device* device, 
			D3D12_CPU_DESCRIPTOR_HANDLE descriptor);

		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const { return m_ShaderResourceView; }

	private:
		D3D12_CPU_DESCRIPTOR_HANDLE m_ShaderResourceView;
	};
}