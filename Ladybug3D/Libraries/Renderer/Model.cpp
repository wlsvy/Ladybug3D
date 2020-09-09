#include "Model.hpp"

using namespace std;
using namespace Ladybug3D::D3D12;

namespace Ladybug3D {

	const shared_ptr<Model> Model::Empty = make_shared<Model>();

	Model::Model()
		: Object("Model")
	{
	}
	Model::Model(const Mesh& mesh)
		: m_Meshes({ mesh })
		, Object("Model")
	{
	}
	Model::Model(std::vector<Mesh>&& meshes)
		: m_Meshes(move(meshes))
		, Object("Model")
	{
	}
	void Model::OnImGui()
	{
	}
}