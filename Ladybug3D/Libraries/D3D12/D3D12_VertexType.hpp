#pragma once
#include <DirectXMath.h>

namespace Ladybug3D::D3D12 {

	struct Vertex3D {
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT2 uv = DirectX::XMFLOAT2(0.0f, 0.0f);
		DirectX::XMFLOAT4 tangent = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	};

	struct Vertex_Color {
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	};
}