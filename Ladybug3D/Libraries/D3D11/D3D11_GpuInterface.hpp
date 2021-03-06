#pragma once
#include <memory>
#include <stdint.h>


struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;

namespace Ladybug3D::D3D11 {
	class Device;

	class GpuInterface {
	public:
		GpuInterface();
		~GpuInterface();

		bool Initialize(void* hwnd, uint32_t width, uint32_t height);

		void SetRenderTarget(
			int numViews,
			ID3D11RenderTargetView* const* renderTargetView,
			ID3D11DepthStencilView* depthStencilView);
		void ResizeMainRenderTarget(uint32_t width, uint32_t height);
		
		ID3D11RenderTargetView* const* GetMainRenderTargetAddr() const;
		ID3D11RenderTargetView* GetMainRenderTarget() const;
		void ClearRenderTargetView(ID3D11RenderTargetView* rtv, const float* clearColor) const;
		void PresentSwapChain() const;

		ID3D11Device* GetDeivce() const;
		ID3D11DeviceContext* GetDeviceContext() const;

	private:
		std::shared_ptr<Device> m_Device;
	};
}