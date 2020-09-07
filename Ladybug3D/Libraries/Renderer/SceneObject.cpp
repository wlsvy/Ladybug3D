#pragma once
#include "SceneObject.hpp"

namespace Ladybug3D {

	SceneObject::SceneObject(const std::string& name = "GameObject")
		: Object(name)
	{
	}
	SceneObject::~SceneObject()
	{
	}
}