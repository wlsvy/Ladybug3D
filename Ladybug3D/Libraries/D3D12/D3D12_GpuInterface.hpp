#pragma once
#include <memory>
#include <stdint.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <Windows.h>

namespace Ladybug3D::D3D12 {
	class Device;

	constexpr UINT FRAME_COUNT = 2;

	class GpuInterface {
	public:
		GpuInterface();
		~GpuInterface();

		virtual bool Initialize(HWND hwnd, UINT width, UINT height);
		virtual void OnUpdate();
		virtual void OnRender();
		virtual void OnDestroy();

		void RenderBegin();
		void RenderEnd();
		void SetRenderTarget(UINT numDesriptors, ID3D12DescriptorHeap* rtv, bool isSingleToRange, const D3D12_CPU_DESCRIPTOR_HANDLE* dsv);
		void ClearRenderTarget(ID3D12DescriptorHeap* rtv, float* clearColor );
		void PresentSwapChain();

		UINT GetWidth() const { return m_Width; }
		UINT GetHeight() const { return m_Height; }
		ID3D12Device* GetDevice() { return m_device.Get(); }
		ID3D12DescriptorHeap* GetRtvDescriptorHeap() { return m_rtvHeap.Get(); }
		ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return m_srvHeap.Get(); }
		ID3D12DescriptorHeap*const* GetSrvDescriptorHeapAddr() { return m_srvHeap.GetAddressOf(); }
		ID3D12GraphicsCommandList* GetCommandList() { return m_commandList.Get(); }


	private:
		void GetDebugInterface();
		void GetAdapters(bool useWarp);
		void CreateDevice(IDXGIAdapter4* adapter);
		void CreateCommandQueue();
		void CreateSwapChain(HWND hwnd);

		void LoadPipeline(HWND hwnd);
		void LoadAssets();
		void PopulateCommandList();
		void WaitForPreviousFrame();

		
		
		

		void GetHardwareAdapter(
			_In_ IDXGIFactory1* pFactory,
			_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
			bool requestHighPerformanceAdapter = false);

		static const UINT FrameCount = 2;

		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12Device> m_device;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
		UINT m_rtvDescriptorSize;

		// Synchronization objects.
		UINT m_frameIndex;
		HANDLE m_fenceEvent;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		UINT64 m_fenceValue;

		UINT m_Width;
		UINT m_Height;
		float m_aspectRatio;

		bool m_useWarpDevice;
	};
}