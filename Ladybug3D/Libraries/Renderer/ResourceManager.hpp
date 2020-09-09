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

	private:
		void LoadTextures();
		void LoadModels();

		std::unordered_map<std::string, std::shared_ptr<Model>> m_ModelMap;
		std::unordered_map<std::string, std::shared_ptr<Ladybug3D::D3D12::Texture>> m_TextureMap;
	};

	Model LoadModel(const std::string& filePath, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

}