#pragma once
#include "D3D12_Define.hpp"
#include "D3D12_Resource.hpp"
#include <d3d12.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include <vector>

namespace DirectX {
	class ResourceUploadBatch;
	class VertexPositionNormalTexture;
}

namespace Ladybug3D::D3D12 {
	class Texture : public Resource {
	public:
		Texture();
		~Texture();

		void InitializeWICTexture(
			const wchar_t* filePath, 
			DirectX::ResourceUploadBatch& uploadBatch, 
			ID3D12Device* device);
		void CreateShaderResourceView(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE descriptor);

	private:
	};

	

	

	

	

	
}