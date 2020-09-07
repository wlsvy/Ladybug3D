#pragma once
#include "SceneObject.hpp"
#include "Transform.hpp"
#include "Util.hpp"
#include "Scene.hpp"

using namespace std;

namespace Ladybug3D {

	SceneObject::SceneObject(const std::string& name)
		: Object(name)
		, m_Transform(make_shared<Transform>(this))
	{
		m_Transform->SetParent(Util::GetCurrentScene().GetWorldTransform());
	}
	SceneObject::~SceneObject()
	{
	}
}