#pragma once
#include "Util.hpp"
#include "Renderer.hpp"
#include "D3D12HelloTriangle.hpp"

namespace Ladybug3D::Util {

	Scene& GetCurrentScene()
	{
		return *D3D12HelloTriangle::s_Ptr->GetCurrentScene();
		//return *Renderer::GetInstance().GetCurrentScene();
	}
}