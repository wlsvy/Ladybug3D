#include "D3D11_Device.hpp"
#include "D3D11_Common.hpp"

using namespace Ladybug3D;
using namespace std;
using namespace Microsoft::WRL;

namespace Ladybug3D::D3D11 {
	Device::Device()
	{
	}

	Device::~Device()
	{
	}

	bool Device::Initialize(HWND hwnd, UINT width, UINT height)
	{
		try {
			CreateAdapters();
			CreateDevice(hwnd, width, height);
			CreateMainRenderTarget();
			SetViewPort(width, height);
		}
		catch (std::exception& e) {
			PrintConsoleLog(e.what());
			return false;
		}

		return true;
	}

	void Device::CreateDevice(HWND hwnd, UINT width, UINT height)
	{
		DXGI_SWAP_CHAIN_DESC scd = { 0 };
		scd.BufferDesc.Width = width;
		scd.BufferDesc.Height = height;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;

		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hwnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ThrowIfFailed(
			D3D11CreateDeviceAndSwapChain(
				GetAdapters().front().Get(),
				D3D_DRIVER_TYPE_UNKNOWN,
				NULL,
				D3D11_CREATE_DEVICE_DEBUG,
				NULL,
				0,
				D3D11_SDK_VERSION,
				&scd,
				this->m_Swapchain.GetAddressOf(),
				this->m_Device.GetAddressOf(),
				NULL,
				this->m_DeviceContext.GetAddressOf()),
			"Failed to create device and swapchain.");
	}

	void Device::CreateAdapters()
	{
		ComPtr<IDXGIFactory> pFactory;

		ThrowIfFailed(
			CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf())),
			"Failed to create DXGIFactory for enumerating adapters.");

		IDXGIAdapter* pAdapter;
		UINT index = 0;

		while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter))) {
			m_Adapters.push_back(pAdapter);
			index++;
		}
	}

	void Device::CreateMainRenderTarget()
	{
		ComPtr<ID3D11Texture2D> backBuffer;
		ThrowIfFailed(
			m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())),
			"Failed To Get BackBuffer.");

		ThrowIfFailed(
			m_Device->CreateRenderTargetView(backBuffer.Get(), NULL, this->m_MainRenderTargetView.GetAddressOf()),
			"Failed to create render target view.");
	}

	void Device::SetViewPort(UINT width, UINT height)
	{
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
		m_DeviceContext->RSSetViewports(1, &viewport);
	}
}



//void DX11Resources::CreateBackBufferAndMainRTV(UINT width, UINT height)
//{
//	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
//	ThrowIfFailed(
//		m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())),
//		"GetBuffer Failed.");
//
//	ThrowIfFailed(
//		m_Device->CreateRenderTargetView(backBuffer.Get(), NULL, this->m_MainRenderTargetView.GetAddressOf()),
//		"Failed to create render target view.");
//
//	//뷰포트 만들기 & 세팅
//	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
//	m_DeviceContext->RSSetViewports(1, &viewport);
//}