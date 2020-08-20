#pragma once
#include <wrl/client.h>
#include <vector>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct IDXGIAdapter;
struct ID3D11RenderTargetView;

namespace Ladybug3D::D3D11 {
	class Device {
	public:
		Device();
		~Device();

		bool Initialize(HWND hwnd, UINT width, UINT height);

		const std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter>>& GetAdapters() { return m_Adapters; }

		ID3D11Device* GetDevice() const { return m_Device.Get(); }
		ID3D11DeviceContext* GetDeviceContext() const { return m_DeviceContext.Get(); }
		IDXGISwapChain* GetSwapChain() const { return m_Swapchain.Get(); }
		ID3D11RenderTargetView* GetMainRenderTarget() const { return m_MainRenderTargetView.Get(); }
		ID3D11RenderTargetView*const* GetMainRenderTargetAddress() const { return m_MainRenderTargetView.GetAddressOf(); }

	private:
		void CreateDevice(HWND hwnd, UINT width, UINT height);
		void InitializeAdapters();
		void CreateMainRenderTarget();
		void SetViewPort(UINT width, UINT height);

		std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter>> m_Adapters;

		Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_Swapchain;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_MainRenderTargetView;
	};
}