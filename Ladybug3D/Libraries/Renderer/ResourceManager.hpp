#pragma once
#include <vector>
#include <memory>
#include <string>
#include <D3D12/D3D12_Define.hpp>

namespace Ladybug3D {

	class Model;

	class ResourceManager {
	public:
		ResourceManager();
		~ResourceManager();
	};

	Model LoadModel(const std::string& filePath, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

}