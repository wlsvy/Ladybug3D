#pragma once
#include "SceneObject.hpp"
#include <DirectXMath.h>

namespace Ladybug3D {

	class Camera : public SceneObject {
	public:
		Camera();
		~Camera();
		void UpdateView();
		void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

		const DirectX::XMMATRIX& GetViewMatrix() const { return m_ViewMatrix; }
		const DirectX::XMMATRIX& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const DirectX::XMMATRIX& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
		float GetViewRange() const { return m_ViewRange; }

	private:
		DirectX::XMMATRIX m_ViewMatrix;
		DirectX::XMMATRIX m_ProjectionMatrix;
		DirectX::XMMATRIX m_ViewProjectionMatrix;

		float m_ViewRange;
	};
}