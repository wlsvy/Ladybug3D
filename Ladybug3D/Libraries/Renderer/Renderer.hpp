#pragma once
#include <wrl/client.h>
#include <memory>

struct IDXGIAdapter4;
struct IDXGISwapChain3;
struct ID3D12Device;
struct ID3D12Resource;
struct ID3D12CommandAllocator;
struct ID3D12CommandQueue;
struct ID3D12DescriptorHeap;
struct ID3D12PipelineState;
struct ID3D12CommandList;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;
struct ID3D12PipelineState;
struct CD3DX12_CPU_DESCRIPTOR_HANDLE;

namespace Ladybug3D::D3D12 {
	class GraphicsCommandList;
}

namespace Ladybug3D::Renderer {

	constexpr UINT SWAPCHAIN_BUFFER_COUNT = 2;

	class Renderer {
	public:
		Renderer();
		~Renderer();

		bool Initialize(HWND hwnd, UINT width, UINT height);
		void Render();
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtvHandle() const;

		void ResizeSwapChainBuffer(UINT width, UINT height);
		void ShutDown();

	private:
		void LoadPipeline(HWND hwnd);
		void LoadAssets();

		void CreateDevice(IDXGIAdapter4* adapter);
		void CreateCommandQueue();
		void CreateSwapChain(HWND hwnd);
		void CreateMainRTV();
		void GetDebugInterface();
		void GetAdapters(bool useWarp);
		void InitImGui(HWND hwnd);

		void RenderBegin();
		void RenderEnd();
		void Pass_Gui();

		void PresentSwapChain(bool isVsync);
		void MoveToNextFrame();

		void ClearMainRTV();
		void ShutDownImGui();

		std::unique_ptr<Ladybug3D::D3D12::GraphicsCommandList> m_GraphicsCommandList;

		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12Device> m_device;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[SWAPCHAIN_BUFFER_COUNT];
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_ImguiSrvHeap;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;

		UINT m_rtvDescriptorSize;

		// Synchronization objects.
		UINT m_FrameIndex;

		UINT m_Width;
		UINT m_Height;
		float m_aspectRatio;

		bool m_useWarpDevice;

		const float m_ClearColor[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
	};
}