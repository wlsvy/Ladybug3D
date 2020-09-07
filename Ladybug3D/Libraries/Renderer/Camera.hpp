#pragma once
#include "Object.hpp"
#include <DirectXMath.h>

namespace Ladybug3D {

	class Camera : public Object {
	public:
		Camera();
		~Camera();
		void UpdateView();
		void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

		const DirectX::XMMATRIX& GetViewMatrix() const { return viewMatrix; }
		const DirectX::XMMATRIX& GetProjectionMatrix() const { return projectionMatrix; }
		const DirectX::XMMATRIX& GetViewProjectionMatrix() const { return viewProjectionMatrix; }
		float GetViewRange() const { return m_ViewRange; }

	private:
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;
		DirectX::XMMATRIX viewProjectionMatrix;

		float m_ViewRange;
	};
}