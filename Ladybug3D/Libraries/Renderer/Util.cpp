#pragma once
#include "Util.hpp"
#include "Renderer.hpp"

namespace Ladybug3D::Util {

	Scene& GetCurrentScene()
	{
		return *Renderer::GetInstance().GetCurrentScene();
	}
}