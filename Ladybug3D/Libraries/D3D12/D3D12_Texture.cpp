#include "D3D12_Texture.hpp"
#include "D3D12_Util.hpp"
#include "D3D12_VertexType.hpp"
#include <Direct12XTK/Include/WICTextureLoader.h>
#include <Direct12XTK/Include/DirectXHelpers.h>
#include <Direct12XTK/Include/ResourceUploadBatch.h>
#include <Direct12XTK/Include/VertexTypes.h>


using namespace DirectX;
using namespace Microsoft::WRL;

namespace Ladybug3D::D3D12 {
	Texture::Texture() 
	{
	}
	Texture::~Texture() 
	{
	}

	void Texture::InitializeWICTexture(
		const wchar_t* filePath, 
		ResourceUploadBatch& uploadBatch, 
		ID3D12Device* device)
	{
		ThrowIfFailed(CreateWICTextureFromFile(device, uploadBatch, filePath, m_Resource.GetAddressOf(), true));
		uploadBatch.Transition(m_Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void Texture::CreateShaderResourceView(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE descriptor)
	{
		DirectX::CreateShaderResourceView(device, m_Resource.Get(), descriptor);
	}
	
    
}