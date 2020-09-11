#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <D3D12/D3D12_Define.hpp>

#include "Singleton.hpp"

namespace Ladybug3D {

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
		void TrackAssetsPath();
		void LoadTextures();
		void LoadModels();
		
		std::unordered_map<std::string, std::filesystem::path> m_ModelPathMap;
		std::unordered_map<std::string, std::filesystem::path> m_TexturePathMap;
		std::unordered_map<std::string, std::filesystem::path> m_ShaderPathMap;

		std::unordered_map<std::string, std::shared_ptr<Model>> m_ModelMap;
		std::unordered_map<std::string, std::shared_ptr<D3D12::Texture>> m_TextureMap;
	};
}