#include "D3D12_Texture.hpp"
#include "D3D12_Util.hpp"
#include <Direct12XTK/Include/WICTextureLoader.h>
#include <Direct12XTK/Include/DirectXHelpers.h>
#include <Direct12XTK/Include/ResourceUploadBatch.h>

#include "d3dx12.h"

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

	void Texture::Intialize(ID3D12Device* device, UINT width, UINT height, UINT16 mipLevels, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flag)
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Format = format;
		desc.Alignment = 0;
		desc.DepthOrArraySize = 1;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags = flag;
		desc.Height = height;
		desc.Width = (UINT64)width;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.MipLevels = mipLevels;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		D3D12_CLEAR_VALUE ClearValue = {};
		ClearValue.Format = format;

		CreateBuffer(device, desc, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, nullptr, &ClearValue);
	}

	void Texture::CreateShaderResourceView(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE descriptor)
	{
		DirectX::CreateShaderResourceView(device, m_Resource.Get(), descriptor);
	}

	void Texture::CreateCubeMapShaderResourceView(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE descriptor)
	{
		DirectX::CreateShaderResourceView(device, m_Resource.Get(), descriptor, true);
	}
	
    
}