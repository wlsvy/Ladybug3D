#pragma once
#include <memory>

namespace Ladybug3D {
	class Scene;
	class Transform;
}

namespace Ladybug3D::Util {

	std::shared_ptr<Scene> GetCurrentScene();
}