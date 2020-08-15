#pragma once
#include <wrl/client.h>
#include <vector>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct IDXGIAdapter;

namespace Ladybug3D::D3D11 {
	class Device {
	public:
		Device();
		~Device();

		bool Initialize(HWND hwnd, UINT width, UINT height);
		const std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter>> & GetAdapters();

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_Swapchain;
		std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter>> m_Adapters;
	};
}