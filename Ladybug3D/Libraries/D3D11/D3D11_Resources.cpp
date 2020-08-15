#include "D3D11_Resources.hpp"

#include <d3d11.h>
#include <array>
#include "D3D11_Device.hpp"
#include "D3D11_Common.hpp"

using namespace std;
using namespace Ladybug3D;
 
namespace Ladybug3D::D3D11 {
	Resources::Resources()
	{
	}

	Resources::~Resources()
	{
	}

	bool Resources::Initialize(HWND hwnd, int width, int height)
	{
		m_Device = make_shared<Device>();

		return m_Device->Initialize(hwnd, width, height);
	}

	void Resources::SetRenderTarget(
		int numViews,
		ID3D11RenderTargetView* const* renderTargetView,
		ID3D11DepthStencilView* depthStencilView)
	{
		std::array<ID3D11RenderTargetView*, MAX_RENDER_TARGET_BINDING_COUNT> prevRtv = { nullptr };
		ID3D11DepthStencilView* prevDsv = nullptr;
		m_Device->GetDeviceContext()->OMGetRenderTargets(MAX_RENDER_TARGET_BINDING_COUNT, prevRtv.data(), &prevDsv);

		std::array<ID3D11RenderTargetView*, MAX_RENDER_TARGET_BINDING_COUNT> rtvArr = { nullptr };
		for (UINT i = 0; i < numViews; i++) {
			rtvArr[i] = renderTargetView[i];
		}

		if (rtvArr != prevRtv || depthStencilView != prevDsv)
		{
			m_Device->GetDeviceContext()->OMSetRenderTargets
			(
				numViews,
				renderTargetView,
				depthStencilView
			);

			//Profiler::GetInstance().BindingCount_RenderTarget++;
		}
	}
	void Resources::ResizeMainRenderTarget(int width, int height)
	{
		//m_Device->GetSwapChain()->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);

	}
	ID3D11RenderTargetView* const* Resources::GetMainRenderTargetAddr() const
	{
		return m_Device->GetMainRenderTargetAddress();
	}
	ID3D11RenderTargetView* Resources::GetMainRenderTarget() const
	{
		return m_Device->GetMainRenderTarget();
	}
	void Resources::ClearRenderTargetView(ID3D11RenderTargetView* rtv, const float* clearColor) const
	{
		m_Device->GetDeviceContext()->ClearRenderTargetView(rtv, clearColor);
	}
	void Resources::PresentSwapChain() const
	{
		m_Device->GetSwapChain()->Present(1, 0);
	}
	ID3D11Device* Resources::GetDeivce() const
	{
		return m_Device->GetDevice();
	}
	ID3D11DeviceContext* Resources::GetDeviceContext() const
	{
		return m_Device->GetDeviceContext();
	}
}

