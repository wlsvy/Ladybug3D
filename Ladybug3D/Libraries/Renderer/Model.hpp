#pragma once
#include <vector>
#include "Object.hpp"
#include "Mesh.hpp"

namespace Ladybug3D {

	class Model : public Object {
	public:

		static const std::shared_ptr<Model> Empty;

		Model();
		Model(const Mesh& mesh);
		Model(std::vector<Mesh>&& meshes);

		auto& GetMeshes() const { return m_Meshes; }
		
		void OnImGui() override;

	private:
		std::vector<Mesh> m_Meshes;
	};

}