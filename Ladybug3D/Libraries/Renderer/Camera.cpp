#pragma once
#include "Camera.hpp"
#include "Transform.hpp"
#include <ImGui/imgui.h>
#include <Windows.h>
#include <iostream>

using namespace DirectX;
using namespace std;

namespace Ladybug3D {

	Camera::Camera() 
		: SceneObject("Camera")
	{
	}
	Camera::~Camera()
	{
	}

	void Camera::UpdateView()
	{
		m_ViewMatrix = m_Transform->GetViewMatrix();
		m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
	}

	void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
	{
		float fovRadians = (fovDegrees / 360.0f) * DirectX::XM_2PI;
		m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
	}

	void Camera::OnUpdate()
	{
		static ImVec2 prevMousePos;

		float dt = 1.0f / ImGui::GetIO().Framerate;
		Transform& tf = *m_Transform;


		if (ImGui::IsMouseDragging(1))
		{
			auto curMousePos = ImGui::GetMousePos();
			auto dragDelta = ImVec2(curMousePos.x - prevMousePos.x, curMousePos.y - prevMousePos.y);
			tf.rotate(dragDelta.y * m_RotateSpeed * dt, dragDelta.x * m_RotateSpeed * dt, 0.0f);
			ImGui::ResetMouseDragDelta(1);
		}

		float speed = m_MoveSpeed;
		if (ImGui::IsKeyDown(VK_SHIFT)) speed = m_FastMoveSpeed;
		if (ImGui::IsKeyDown('W')) tf.translate(tf.GetForwardVector() * speed * dt);
		if (ImGui::IsKeyDown('S')) tf.translate(tf.GetBackwardVector() * speed * dt);
		if (ImGui::IsKeyDown('A')) tf.translate(tf.GetLeftVector() * speed * dt);
		if (ImGui::IsKeyDown('D')) tf.translate(tf.GetRightVector() * speed * dt);

		prevMousePos = ImGui::GetMousePos();
	}
}