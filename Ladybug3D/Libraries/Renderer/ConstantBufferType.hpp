#pragma once
#include <DirectXMath.h>

namespace Ladybug3D {
	//per frame update
	struct alignas(256) CB_Matrix {
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX viewProj;
		DirectX::XMMATRIX worldViewProj;
	};
	struct alignas(256) CB_Test {
		unsigned int index;
	};
	struct alignas(256) CB_PerObject {
		DirectX::XMMATRIX worldMatrix;
		DirectX::XMMATRIX curWvpMatrix;
		DirectX::XMMATRIX prevWvpWorld;
	};
	struct alignas(256) CB_PerScene {
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projMatrix;
		DirectX::XMMATRIX viewProjMatrix;
	};
}