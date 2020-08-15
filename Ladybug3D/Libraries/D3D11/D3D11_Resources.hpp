#pragma once
#include <memory>
#include <Windows.h>

struct ID3D11Device;

namespace Ladybug3D::D3D11 {
	class Device;

	class Resources {
	public:
		Resources();
		~Resources();

		bool Initialize(HWND hwnd, UINT width, UINT height);

	private:
		std::shared_ptr<Device> m_Device;
	};
}