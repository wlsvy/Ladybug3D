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
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX prevWorld;
	};
	struct alignas(256) CB_PerScene {
		DirectX::XMMATRIX world;
	};
}