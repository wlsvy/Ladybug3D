#pragma once
#include "Scene.hpp"
#include "Transform.hpp"
#include "Camera.hpp"
#include <ImGui/imgui.h>

using namespace std;
using namespace DirectX;

namespace Ladybug3D {

	Scene::Scene()
		: m_WorldTransform(make_shared<Transform>())
	{
		//m_MainCam = make_shared<Camera>();
	}
	Scene::~Scene()
	{
	}

	void Scene::ProcessGuiHirarchy(std::shared_ptr<Transform> transform) const
	{
		bool check = false;
		if (auto selected = m_GuiSelectedObj.lock())
		{
			check = selected == transform;
		}

		ImGuiTreeNodeFlags nodeFlags =
			ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| (check ? ImGuiTreeNodeFlags_Selected : 0)
			| (transform->GetChildNum() == 0 ? ImGuiTreeNodeFlags_Leaf : 0)
			| ImGuiTreeNodeFlags_DefaultOpen;

		bool isNodeOpen = ImGui::TreeNodeEx(transform->GetSceneObject()->Name.c_str(), nodeFlags);
		if (ImGui::IsItemClicked())
		{
			m_GuiSelectedObj = transform;
		}

		if (isNodeOpen)
		{
			for (auto child : transform->m_Children)
			{
				ProcessGuiHirarchy(child);
			}
			ImGui::TreePop();
		}
	}

	void Scene::OnImGui()
	{
		for (auto child : m_WorldTransform->m_Children)
		{
			ProcessGuiHirarchy(child);
		}
		ImGui::Spacing();
	}

	void Scene::Initialize()
	{

	}

	void Scene::OnUpdate()
	{
		m_WorldTransform->UpdateMatrix(XMMatrixIdentity(), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	}
}