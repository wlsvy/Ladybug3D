#pragma once
#include <wrl/client.h>
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <D3D12/D3D12_Define.hpp>

namespace Ladybug3D::Renderer {

	constexpr UINT SWAPCHAIN_BUFFER_COUNT = 2;

	struct alignas(256) CB_Matrix {
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 viewProj;
		DirectX::XMFLOAT4X4 prevMvp;
	}; 
	struct alignas(256) CB_Test {
		UINT index;
	};

	class Renderer {
	public:
		Renderer();
		~Renderer();

		bool Initialize(HWND hwnd, UINT width, UINT height);

		void LoadAssetsBegin();
		void LoadTexture(const wchar_t * filePath);
		void LoadAssetsEnd();

		void Render();

		void ResizeSwapChainBuffer(UINT width, UINT height);
		void ShutDown();

	private:
		void LoadAssets();

		void CreateDevice(IDXGIAdapter4* adapter);
		void CreateCommandQueue();
		void CreateSwapChain(HWND hwnd);
		void CreateMainRTV();
		void CreateResourceView();
		void GetDebugInterface();
		void GetAdapters(bool useWarp);
		void InitImGui(HWND hwnd);
		void CreateRootSignature();

		void Update();
		void RenderBegin();
		void RenderEnd();
		void Pass_Gui();

		void PresentSwapChain(bool isVsync);
		void MoveToNextFrame();

		void ClearMainRTV();
		void ShutDownImGui();

		std::unique_ptr<Ladybug3D::D3D12::GraphicsCommandList> m_GraphicsCommandList;
		std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_ResourceDescriptorHeap;
		std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_MainRTVDescriptorHeap;
		std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_ImGuiDescriptorHeap;
		std::unique_ptr<Ladybug3D::D3D12::Texture> m_SampleTexture;
		std::unique_ptr<Ladybug3D::D3D12::ConstantBuffer<CB_Matrix>> m_CbMatrix;
		std::unique_ptr<Ladybug3D::D3D12::ConstantBuffer<CB_Test>> m_CbTest;

		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[SWAPCHAIN_BUFFER_COUNT];
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_ImguiSrvHeap;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

		UINT m_FrameIndex;

		UINT m_Width = 0;
		UINT m_Height = 0;
		float m_aspectRatio;

		bool m_useWarpDevice;

		const float m_ClearColor[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
	};
}