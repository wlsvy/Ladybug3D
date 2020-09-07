#pragma once
#include "Scene.hpp"
#include "Transform.hpp"
#include "Camera.hpp"

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

	void Scene::Initialize()
	{
		/*Core::Find<GameObject>("X_Bot")->GetRenderInfo().Anim->SetClip(Core::Find<AnimationClip>("X_Bot_Idle"));
		Core::Find<GameObject>("X_Bot")->GetRenderInfo().Anim->Play();

		{
			auto sphere = Core::CreateInstance<GameObject>("Cube");
			sphere->GetTransform().SetPosition(3.0f, 5.0f, 2.9f);
			sphere->GetRenderInfo().SetModel(Core::Find<Model>("Box"));
			sphere->GetRenderInfo().SetMaterial(0, Core::Find<SharedMaterial>("AluminiumInsulator"));
		}
		{
			auto sphere = Core::CreateInstance<GameObject>("Furry");
			sphere->GetTransform().SetPosition(2.0f, 5.0f, -3.f);
			sphere->GetTransform().SetScale(3.0f, 3.0f, 3.f);
			sphere->GetRenderInfo().SetModel(Core::Find<Model>("torus"));
			sphere->GetRenderInfo().SetMaterial(0, Core::Find<SharedMaterial>("FurTest"));
			sphere->GetRenderInfo().SetShaderState(0, Core::Find<ShaderState>("FurMesh"));
		}

		m_MainCam = Core::CreateInstance<Camera>();
		m_MainCam->GetTransform().SetPosition(-2.6f, 11.3f, -8.6f);
		m_MainCam->GetTransform().SetRotation(40.0f, 30.0f, 0.0f);
		m_MainCam->SetProjectionValues(
			90.0f,
			Engine::Get().GetGraphics().GetWindowWidth() / Engine::Get().GetGraphics().GetWindowHeight(),
			0.1f,
			1000.0f);
		m_MainCam->AddComponent<CamMove>();*/
	}

	void Scene::Update()
	{
		m_WorldTransform->UpdateMatrix(XMMatrixIdentity(), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_MainCam->UpdateView();
	}
}