#pragma once
#include "Object.hpp"

namespace Ladybug3D {
	class Scene;
	class Transform;
}

namespace Ladybug3D::Util {

	std::shared_ptr<Scene> GetCurrentScene();
}