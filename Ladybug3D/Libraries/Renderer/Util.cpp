#pragma once
#include "Util.hpp"
#include "Renderer.hpp"

namespace Ladybug3D::Util {

	std::shared_ptr<Scene> GetCurrentScene()
	{
		return Renderer::GetInstance().GetCurrentScene();
	}
}