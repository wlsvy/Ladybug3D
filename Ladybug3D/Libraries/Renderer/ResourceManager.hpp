#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <D3D12/D3D12_Define.hpp>

#include "Singleton.hpp"

namespace Ladybug3D {

	namespace D3D12 {
		class Texture;
	}

	class Model;

	class ResourceManager : public Singleton<ResourceManager> {
	public:
		ResourceManager();
		~ResourceManager();

		bool Initialize();
		auto& GetTextureMap() const { return m_TextureMap; }
		auto& GetModelMap() const { return m_ModelMap; }
		auto GetModel(const std::string& name) { return m_ModelMap[name]; }
		auto GetTexture(const std::string& name) { return m_TextureMap[name]; }

	private:
		void LoadTextures();
		void LoadModels();

		std::unordered_map<std::string, std::shared_ptr<Model>> m_ModelMap;
		std::unordered_map<std::string, std::shared_ptr<D3D12::Texture>> m_TextureMap;
	};

	std::shared_ptr<Model> LoadModel(const std::string& filePath, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

}