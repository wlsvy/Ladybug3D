#pragma once
#include "Camera.hpp"
#include "Transform.hpp"

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
}