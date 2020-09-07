#pragma once
#include "Object.hpp"

namespace Ladybug3D {

	class SceneObject : public Object {
	public:
		SceneObject(const std::string& name = "GameObject");
		~SceneObject();
	};
}