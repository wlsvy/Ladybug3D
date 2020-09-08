#pragma once
#include "Camera.hpp"
#include "Transform.hpp"
#include <ImGui/imgui.h>
#include <Windows.h>

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
		float dt = 1.0f / ImGui::GetIO().Framerate;
		Transform& tf = *m_Transform;

		auto dragDelta = ImGui::GetMouseDragDelta(1);
		if (abs(dragDelta.x) > 0.01 || abs(dragDelta.y) > 0.01) {
			tf.rotate(dragDelta.y * m_RotateSpeed * dt, dragDelta.x * m_RotateSpeed * dt, 0.0f);
			ImGui::ResetMouseDragDelta(1);
		}

		float speed = m_MoveSpeed;
		if (ImGui::IsKeyDown(VK_SHIFT)) speed = m_FastMoveSpeed;
		if (ImGui::IsKeyDown('W')) tf.translate(tf.GetForwardVector() * speed * dt);
		if (ImGui::IsKeyDown('S')) tf.translate(tf.GetBackwardVector() * speed * dt);
		if (ImGui::IsKeyDown('A')) tf.translate(tf.GetLeftVector() * speed * dt);
		if (ImGui::IsKeyDown('D')) tf.translate(tf.GetRightVector() * speed * dt);
	}
}