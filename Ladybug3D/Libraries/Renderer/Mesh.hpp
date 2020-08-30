#pragma once
#include <memory>
#include <map>
#include <D3D12/D3D12_Define.hpp>

namespace Ladybug3D::Renderer {

	class ManagerBase {
	public:
		ManagerBase();
		~ManagerBase();
		void CreateRootSignature(ID3D12Device* device);

	protected:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_DescriptorHeap;
	};

	class TextureManager : public ManagerBase {
	public:

		void AddTexture(const wchar_t* filePath);
		void Erase(UINT offset);
		void Clear();
		//auto GetShaderResourceView(UINT offset) { return m_DescriptorHeap->GetGpuHandle(offset); }
		void SetDescriptorHandle(D3D12::GraphicsCommandList* cmdList, UINT offset);

	private:
		std::map<int, std::shared_ptr<Ladybug3D::D3D12::Texture>> m_TextureMap;
	};

	class CBVManager {

	};

	class ShaderManager {

	};
}