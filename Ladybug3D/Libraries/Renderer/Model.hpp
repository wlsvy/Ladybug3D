#pragma once
#include <vector>
#include "Mesh.hpp"

namespace Ladybug3D {

	class Model {
	public:
		Model() {}
		Model(const Mesh& mesh) : m_Meshes({ mesh }) {}
		Model(std::vector<Mesh> && meshes) : m_Meshes(std::move(meshes)) {}

		auto& GetMeshes() const { return m_Meshes; }

	private:
		std::vector<Mesh> m_Meshes;
	};

}