#include "Renderer.hpp"
#include <iostream>
#include <algorithm>
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <sal.h>

#include <D3D12/d3dx12.h>
#include <D3D12/D3D12_Util.hpp>
#include <D3D12/D3D12_CommandList.hpp>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx12.h>
#include <ImGui/imgui_impl_win32.h>

using namespace std;
using namespace Microsoft::WRL;
using namespace Ladybug3D::D3D12;

namespace Ladybug3D::Renderer {

	Renderer::Renderer() 
	{
	}
	Renderer::~Renderer()
	{
	}

	bool Renderer::Initialize(HWND hwnd, UINT width, UINT height)
	{
		try {
			m_Width = width;
			m_Height = height;
			m_aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

			LoadPipeline(hwnd);

			m_GraphicsCommandList = make_unique<GraphicsCommandList>(m_device.Get());
			InitImGui(hwnd);
		}
		catch (exception& e) {
			cout << e.what() << endl;
			return false;
		}
		return true;
	}

	void Renderer::Render()
	{
		RenderBegin();
		m_GraphicsCommandList->SetRenderTarget(1, &GetRtvHandle());
		m_GraphicsCommandList->ClearRenderTarget(GetRtvHandle(), m_ClearColor);
		Pass_Gui();
		RenderEnd();
		PresentSwapChain(true);
		MoveToNextFrame();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE Renderer::GetRtvHandle() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_rtvDescriptorSize);
	}

	void Renderer::ResizeSwapChainBuffer(UINT width, UINT height)
	{
		if (m_Width == width && m_Height == height) return;

		m_Width = max(1u, width);
		m_Height = max(1u, height);
		m_aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

		ClearMainRTV();

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ThrowIfFailed(m_swapChain->GetDesc(&swapChainDesc));
		ThrowIfFailed(m_swapChain->ResizeBuffers(
			SWAPCHAIN_BUFFER_COUNT, 
			m_Width, m_Height,
			swapChainDesc.BufferDesc.Format, 
			swapChainDesc.Flags));

		m_FrameIndex = m_swapChain->GetCurrentBackBufferIndex();

		CreateMainRTV();
	}

	void Renderer::ShutDown()
	{
		ShutDownImGui();
	}

	void Renderer::GetDebugInterface()
	{
		ComPtr<ID3D12Debug> debugInterface;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
		debugInterface->EnableDebugLayer();
	}

	_Use_decl_annotations_
	void GetHardwareAdapter(
		IDXGIFactory1* pFactory,
		IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = false)
	{
		*ppAdapter = nullptr;

		ComPtr<IDXGIAdapter1> adapter;

		ComPtr<IDXGIFactory6> factory6;
		if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (
				UINT adapterIndex = 0;
				DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
					IID_PPV_ARGS(&adapter));
				++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					continue;
				}

				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}
		else
		{
			for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					continue;
				}
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		*ppAdapter = adapter.Detach();
	}

	void Renderer::GetAdapters(bool useWarp)
	{
		ComPtr<IDXGIFactory4> factory;
		UINT createFactoryFlags = 0;
#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory)));

		ComPtr<IDXGIAdapter1> adapter1;
		if (useWarp)
		{
			ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter1)));
			ThrowIfFailed(adapter1.As(&m_Adapter));
		}
		else
		{
			GetHardwareAdapter(factory.Get(), adapter1.GetAddressOf());
			ThrowIfFailed(adapter1.As(&m_Adapter));
		}
	}

	void Renderer::InitImGui(HWND hwnd)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(
			m_device.Get(),
			SWAPCHAIN_BUFFER_COUNT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			m_ImguiSrvHeap.Get(),
			m_ImguiSrvHeap->GetCPUDescriptorHandleForHeapStart(),
			m_ImguiSrvHeap->GetGPUDescriptorHandleForHeapStart());
	}

	void Renderer::CreateDevice(IDXGIAdapter4* adapter)
	{
		ComPtr<ID3D12Device2> d3d12Device2;
		ThrowIfFailed(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_device.GetAddressOf())));

#if defined(_DEBUG)
		ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(m_device.As(&pInfoQueue)))
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			D3D12_MESSAGE_ID DenyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
		}
#endif
	}

	void Renderer::CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
	}

	void Renderer::CreateSwapChain(HWND hwnd)
	{
		UINT createFactoryFlags = 0;
#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		ComPtr<IDXGIFactory4> factory4;
		ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory4)));

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = SWAPCHAIN_BUFFER_COUNT;
		swapChainDesc.Width = m_Width;
		swapChainDesc.Height = m_Height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain1;
		ThrowIfFailed(factory4->CreateSwapChainForHwnd(
			m_commandQueue.Get(),
			hwnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1));

		// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen will be handled manually.
		ThrowIfFailed(factory4->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
		ThrowIfFailed(swapChain1.As(&m_swapChain));
	}

	void Renderer::CreateMainRTV()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		for (UINT n = 0; n < SWAPCHAIN_BUFFER_COUNT; n++)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
			m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}

	void Renderer::LoadPipeline(HWND hwnd)
	{
		UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
		GetDebugInterface();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		GetAdapters(false);
		CreateDevice(m_Adapter.Get());
		CreateCommandQueue();
		CreateSwapChain(hwnd);


		// Create descriptor heaps.
		{
			// Describe and create a render target view (RTV) descriptor heap.
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.NumDescriptors = SWAPCHAIN_BUFFER_COUNT;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtvHeap)));

			m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.NumDescriptors = 1;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_ImguiSrvHeap)));
		}

		CreateMainRTV();
	}

	void Renderer::LoadAssets() {
		// Create the pipeline state, which includes compiling and loading shaders.
		//{
		//	struct
		//	{
		//		uint8_t* data;
		//		uint32_t size;
		//	} meshShader, pixelShader;

		//	ReadDataFromFile(GetAssetFullPath(c_meshShaderFilename).c_str(), &meshShader.data, &meshShader.size);
		//	ReadDataFromFile(GetAssetFullPath(c_pixelShaderFilename).c_str(), &pixelShader.data, &pixelShader.size);

		//	// Pull root signature from the precompiled mesh shader.
		//	ThrowIfFailed(m_device->CreateRootSignature(0, meshShader.data, meshShader.size, IID_PPV_ARGS(&m_rootSignature)));

		//	D3DX12_MESH_SHADER_PIPELINE_STATE_DESC psoDesc = {};
		//	psoDesc.pRootSignature = m_rootSignature.Get();
		//	psoDesc.MS = { meshShader.data, meshShader.size };
		//	psoDesc.PS = { pixelShader.data, pixelShader.size };
		//	psoDesc.NumRenderTargets = 1;
		//	psoDesc.RTVFormats[0] = m_renderTargets[0]->GetDesc().Format;
		//	psoDesc.DSVFormat = m_depthStencil->GetDesc().Format;
		//	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);    // CW front; cull back
		//	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);         // Opaque
		//	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Less-equal depth test w/ writes; no stencil
		//	psoDesc.SampleMask = UINT_MAX;
		//	psoDesc.SampleDesc = DefaultSampleDesc();

		//	auto psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(psoDesc);

		//	D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
		//	streamDesc.pPipelineStateSubobjectStream = &psoStream;
		//	streamDesc.SizeInBytes = sizeof(psoStream);

		//	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&streamDesc, IID_PPV_ARGS(&m_pipelineState)));
		//}
	}

	void Renderer::PresentSwapChain(bool isVsync)
	{
		ThrowIfFailed(m_swapChain->Present(isVsync, 0));
		
	}

	void Renderer::MoveToNextFrame()
	{
		auto fence = m_GraphicsCommandList->GetFence();
		auto fenceValue = m_GraphicsCommandList->GetAndIncreaseFenceValue();
		auto fenceEvent = m_GraphicsCommandList->GetFenceEvent();

		ThrowIfFailed(m_commandQueue->Signal(fence, fenceValue));
		if (fence->GetCompletedValue() < fenceValue)
		{
			ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
			WaitForSingleObject(fenceEvent, INFINITE);
		}
	}

	void Renderer::ClearMainRTV()
	{
		for (auto& rtv : m_renderTargets) {
			rtv.Reset();
		}
	}

	void Renderer::ShutDownImGui()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	
	void Renderer::RenderBegin()
	{
		m_FrameIndex = m_swapChain->GetCurrentBackBufferIndex();

		m_GraphicsCommandList->BeginRenderPass();
		m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	}

	void Renderer::RenderEnd()
	{
		m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		m_GraphicsCommandList->EndRenderPass();

		ID3D12CommandList* ppCommandLists[] = { m_GraphicsCommandList->GetCommandList() };
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}
	void Renderer::Pass_Gui()
	{
		static bool show_demo_window = true;
		static bool show_another_window = false;
		static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");

			ImGui::Text("This is some useful text.");
			ImGui::Checkbox("Demo Window", &show_demo_window);
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			ImGui::ColorEdit3("clear color", (float*)&clear_color);

			if (ImGui::Button("Button"))
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		
		ID3D12DescriptorHeap* heaps[] = { m_ImguiSrvHeap.Get() };
		m_GraphicsCommandList->GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
		
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_GraphicsCommandList->GetCommandList());
	}
}


