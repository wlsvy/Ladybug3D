#pragma once
#include "Object.hpp"

namespace Ladybug3D {

	class Transform;

	class SceneObject : public Object {
	public:
		SceneObject(const std::string& name = "GameObject");
		~SceneObject();

		auto& GetTransform() { return m_Transform; }

	protected:
		std::shared_ptr<Transform> m_Transform;
	};
}