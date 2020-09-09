#pragma once
#include "Object.hpp"

namespace Ladybug3D {

	class Transform;
	class Model;

	class SceneObject : public Object {
	public:
		SceneObject(const std::string& name = "GameObject");
		~SceneObject();

		auto& GetTransform() { return m_Transform; }
		
		std::shared_ptr<Model> Model;

	protected:
		std::shared_ptr<Transform> m_Transform;
	};
}