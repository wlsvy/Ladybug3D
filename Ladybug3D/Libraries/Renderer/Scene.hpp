#pragma once
#include "Object.hpp"
#include <vector>

namespace Ladybug3D {

	class Transform;
	class Camera;
	class Renderer;
	class SceneObject;

	class Scene : public Object {
		friend class Renderer;
	public:
		static constexpr float MAX_SIZE = 9999.0f;
		static constexpr float MIN_SIZE = -9999.0f;

		Scene();
		~Scene();

		std::shared_ptr<Transform> GetWorldTransform() const { return m_WorldTransform; }
		std::shared_ptr<Camera> GetMainCam() const { return m_MainCam; }
	
		void OnDestroy() override;

		void OnImGui() override;
		auto GetGuiSelected() const { return m_GuiSelectedObj; }

	private:
		void Initialize();
		void OnUpdate() override;
		void ProcessGuiHirarchy(std::shared_ptr<Transform>) const;

		std::shared_ptr<Transform> m_WorldTransform;
		std::shared_ptr<Camera> m_MainCam;
		std::vector<std::shared_ptr<SceneObject>> m_SceneObjects;

		mutable std::weak_ptr<Transform> m_GuiSelectedObj;
	};
}