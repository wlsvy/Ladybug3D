#pragma once
#include "Object.hpp"

namespace Ladybug3D {

	class Transform;
	class Camera;
	class Renderer;

	class Scene : public Object {
		friend class Renderer;
		friend class RendererV2;
	public:
		static constexpr float MAX_SIZE = 9999.0f;
		static constexpr float MIN_SIZE = -9999.0f;

		Scene();
		~Scene();

		std::shared_ptr<Transform> GetWorldTransform() const { return m_WorldTransform; }
		std::shared_ptr<Camera> GetMainCam() const { return m_MainCam; }
	
	private:
		void Initialize();
		void OnUpdate() override;

		std::shared_ptr<Transform> m_WorldTransform;
		std::shared_ptr<Camera> m_MainCam;
	};
}