#pragma once
#include "Object.hpp"

namespace Ladybug3D {

	class Transform;
	class Camera;

	class Scene : public Object {
	public:
		static constexpr float MAX_SIZE = 9999.0f;
		static constexpr float MIN_SIZE = -9999.0f;

		Scene();
		~Scene();
	private:
		void Initialize();

		void Update();

		std::shared_ptr<Transform> GetWorldTransform() const { return m_WorldTransform; }
		std::shared_ptr<Camera> GetMainCam() const { return m_MainCam; }
		std::weak_ptr<Transform> GetGuiSelected() const { return m_GuiSelectedObj; }

	private:
		void AwakeGameObject();

		static bool s_IsInitialized;

		std::shared_ptr<Transform> m_WorldTransform;
		std::shared_ptr<Camera> m_MainCam;

		std::weak_ptr<Transform> m_GuiSelectedObj;
	};
}