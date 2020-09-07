#pragma once
#include <vector>
#include <DirectXMath.h>
#include "Object.hpp"

namespace Ladybug3D {

	constexpr float POSITION_MAX = 10000.0f;
	constexpr float POSITION_MIN = -10000.0f;
	constexpr float Deg2Rad = 0.0174533f;	// pi / 180
	constexpr float Rad2Deg = 57.2958f;	// 180 / pi

	class Scene;
	class SceneObject;

	class Transform : public Object {
		friend class Scene;
	public:
		Transform();
		Transform(const SceneObject* sceneObj);
		~Transform();

		void SetPosition(const DirectX::XMVECTOR& pos) { positionVec = pos; }
		void SetPosition(const DirectX::XMFLOAT3& pos) { position = pos; }
		void SetPosition(float x, float y, float z) { position = DirectX::XMFLOAT3(x, y, z); }
		void SetRotation(const DirectX::XMFLOAT3& rot) { rotation = rot; }
		void SetRotation(const DirectX::XMVECTOR& rot) { rotationVec = rot; }
		void SetRotation(float x, float y, float z) { rotation = DirectX::XMFLOAT3(x, y, z); }
		void SetScale(const DirectX::XMVECTOR& s) { scaleVec = s; }
		void SetScale(const DirectX::XMFLOAT3& s) { scale = s; }
		void SetScale(float x, float y, float z) { scale = DirectX::XMFLOAT3(x, y, z); }
		void SetLookAtPos(DirectX::XMFLOAT3 lookAtPos);

		void translate(const DirectX::XMVECTOR& pos) { using DirectX::operator+=; positionVec += pos; }
		void translate(const DirectX::XMFLOAT3& pos) { using DirectX::operator+=; positionVec += DirectX::XMVectorSet(pos.x, pos.y, pos.z, 0.0f); }
		void translate(float x, float y, float z) { using DirectX::operator+=; positionVec += DirectX::XMVectorSet(x, y, z, 0.0f); }
		void rotate(const DirectX::XMVECTOR& rot) { using DirectX::operator+=; rotationVec += rot; }
		void rotate(const DirectX::XMFLOAT3& rot) { using DirectX::operator+=; rotationVec += DirectX::XMVectorSet(rot.x, rot.y, rot.z, 0.0f); }
		void rotate(float x, float y, float z) { using DirectX::operator+=; rotationVec += DirectX::XMVectorSet(x, y, z, 0.0f); }

		DirectX::XMVECTOR GetGlobalPosition() const { return m_GlobalPositionVec; }
		DirectX::XMVECTOR GetGlobalQuaternion() const { return m_GlobalQuaternionVec; }
		DirectX::XMVECTOR GetLossyScale() const { return m_GlobalLossyScaleVec; }
		DirectX::XMVECTOR GetQuaternion() const { using DirectX::operator*;  return DirectX::XMQuaternionRotationRollPitchYawFromVector(rotationVec * Deg2Rad); }

		const DirectX::XMVECTOR& GetForwardVector() const { return m_Forward; }
		const DirectX::XMVECTOR& GetUpwardVector() const { return m_Upward; }
		const DirectX::XMVECTOR& GetLeftVector() const { return m_Left; }
		DirectX::XMVECTOR GetBackwardVector() const { using DirectX::operator*; return m_Forward * -1; }
		DirectX::XMVECTOR GetDownwardVector() const { using DirectX::operator*; return m_Upward * -1; }
		DirectX::XMVECTOR GetRightVector() const { using DirectX::operator*; return m_Left * -1; }

		const DirectX::XMMATRIX& GetWorldMatrix() const { return m_WorldMatrix; }
		DirectX::XMMATRIX		 GetViewMatrix() const { using DirectX::operator+; return DirectX::XMMatrixLookAtLH(positionVec, m_Forward + positionVec, m_Upward); }

		std::shared_ptr<Transform> GetParent() const { return m_Parent; }
		std::shared_ptr<Transform> GetChild(int index) const { return m_Children[index]; }
		size_t GetChildNum() const { return m_Children.size(); }

		void SetParent(const std::shared_ptr<Transform>& transform);
		bool HaveChildTransform(Transform* _transform);

		auto GetSceneObject() { return m_SceneObject; }
		void OnImGui() override;

		union {
			DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMVECTOR positionVec;
		};
		union {
			DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMVECTOR rotationVec;
		};
		union {
			DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
			DirectX::XMVECTOR scaleVec;
		};


		static const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR;
		static const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR;
		static const DirectX::XMVECTOR DEFAULT_UP_VECTOR;
		static const DirectX::XMVECTOR DEFAULT_DOWN_VECTOR;
		static const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR;
		static const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR;

	private:
		void UpdateMatrix(
			const DirectX::XMMATRIX& parentm_WorldMatrix,
			const DirectX::XMVECTOR& parentQuat);
		void UpdateDirectionVectors(const DirectX::XMMATRIX& rotationMat);

		void SetChild(const std::shared_ptr<Transform>& child);
		void EraseChild(Transform* child);
		DirectX::XMVECTOR CalculateLossyScale() const;

		union {
			DirectX::XMFLOAT3 m_GlobalPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMVECTOR m_GlobalPositionVec;
		};
		union {
			DirectX::XMFLOAT4 m_GlobalQuaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			DirectX::XMVECTOR m_GlobalQuaternionVec;
		};
		union {
			DirectX::XMFLOAT3 m_GlobalLossyScale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
			DirectX::XMVECTOR m_GlobalLossyScaleVec;
		};

		DirectX::XMMATRIX m_WorldMatrix = DirectX::XMMatrixIdentity();
		DirectX::XMVECTOR m_Forward;
		DirectX::XMVECTOR m_Left;
		DirectX::XMVECTOR m_Upward;

		std::shared_ptr<Transform> m_Parent;
		std::vector<std::shared_ptr<Transform>> m_Children;
		const SceneObject* m_SceneObject;
	};

}