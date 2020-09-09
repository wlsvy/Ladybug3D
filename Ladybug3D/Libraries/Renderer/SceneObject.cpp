#pragma once
#include "SceneObject.hpp"
#include "Transform.hpp"
#include "Util.hpp"
#include "Scene.hpp"
#include "Model.hpp"

#include <ImGui/imgui.h>

using namespace std;

namespace Ladybug3D {

	SceneObject::SceneObject(const std::string& name)
		: Object(name)
		, m_Transform(make_shared<Transform>(this))
		, Model(Model::Empty)
	{
		m_Transform->SetParent(Util::GetCurrentScene()->GetWorldTransform());
	}

	SceneObject::~SceneObject()
	{
		m_Transform->SetParent(nullptr);
	}

	void SceneObject::OnImGui()
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll;
		ImGui::InputText("Name", &Name[0], flags);
		ImGui::SameLine();
		ImGui::Text("(ID : %d)", GetId());

		ImGui::Spacing();
		ImGui::BeginChild(GetId(), ImVec2(0, 0), false); // ID는 0 이 되면 안됨

		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_CollapsingHeader
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_DefaultOpen;

		if (ImGui::CollapsingHeader(m_Transform->Name.c_str(), node_flags))
		{
			ImGui::Spacing();
			m_Transform->OnImGui();
		}
		ImGui::Spacing();

		if (ImGui::CollapsingHeader(Model->Name.c_str(), node_flags))
		{
			ImGui::Spacing();
			Model->OnImGui();
		}
		ImGui::Spacing();

		ImGui::Separator();
		ImGui::Spacing();
		ImGui::EndChild();
	}
}