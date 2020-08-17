#pragma once
#include <memory>
#include <stdint.h>

struct ID3D12Device;
struct ID3D12DeviceContext;
struct ID3D12DepthStencilView;
struct ID3D12RenderTargetView;

namespace Ladybug3D::D3D12 {
	class Device;

	class GpuInterface {
	public:
		GpuInterface();
		~GpuInterface();

	private:
	};
}