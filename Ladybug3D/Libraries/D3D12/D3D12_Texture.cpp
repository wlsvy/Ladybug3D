#include "D3D12_Texture.hpp"
#include "d3dx12.h"
#include "D3D12_Util.hpp"
#include <Direct12XTK/Include/WICTextureLoader.h>
#include <Direct12XTK/Include/DirectXHelpers.h>

using namespace DirectX;

namespace Ladybug3D::D3D12 {
	Texture::Texture() 
	{
	}
	Texture::~Texture() 
	{
	}

	void Texture::InitializeWICTexture
	(const wchar_t* filePath, 
		ResourceUploadBatch& uploadBatch, 
		ID3D12Device* device, 
		D3D12_CPU_DESCRIPTOR_HANDLE descriptor)
	{
		ThrowIfFailed(CreateWICTextureFromFile(device, uploadBatch, filePath, m_Resource.GetAddressOf(), true));
		CreateShaderResourceView(device, m_Resource.Get(), descriptor);

		m_ShaderResourceView = descriptor;
	}
}