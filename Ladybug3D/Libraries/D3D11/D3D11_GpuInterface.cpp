#include "D3D11_GpuInterface.hpp"

#include <Windows.h>
#include <d3d11.h>
#include <array>
#include "D3D11_Device.hpp"
#include "D3D11_Common.hpp"

using namespace std;
using namespace Ladybug3D;
 
namespace Ladybug3D::D3D11 {
	GpuInterface::GpuInterface() : m_Device(make_shared<Device>())
	{
	}

	GpuInterface::~GpuInterface()
	{
	}

	bool GpuInterface::Initialize(void* hwnd, uint32_t width, uint32_t height)
	{
		return m_Device->Initialize(static_cast<HWND>(hwnd), width, height);
	}

	void GpuInterface::SetRenderTarget(
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
	void GpuInterface::ResizeMainRenderTarget(uint32_t width, uint32_t height)
	{
		//m_Device->GetSwapChain()->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);

	}
	ID3D11RenderTargetView* const* GpuInterface::GetMainRenderTargetAddr() const
	{
		return m_Device->GetMainRenderTargetAddress();
	}
	ID3D11RenderTargetView* GpuInterface::GetMainRenderTarget() const
	{
		return m_Device->GetMainRenderTarget();
	}
	void GpuInterface::ClearRenderTargetView(ID3D11RenderTargetView* rtv, const float* clearColor) const
	{
		m_Device->GetDeviceContext()->ClearRenderTargetView(rtv, clearColor);
	}
	void GpuInterface::PresentSwapChain() const
	{
		m_Device->GetSwapChain()->Present(1, 0);
	}
	ID3D11Device* GpuInterface::GetDeivce() const
	{
		return m_Device->GetDevice();
	}
	ID3D11DeviceContext* GpuInterface::GetDeviceContext() const
	{
		return m_Device->GetDeviceContext();
	}
}

