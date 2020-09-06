#include "Renderer.hpp"
#include "Mesh.hpp"
#include <iostream>
#include <algorithm>
#include <filesystem>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <sal.h>
#include <D3Dcompiler.h>

#include <D3D12/d3dx12.h>
#include <D3D12/D3D12_Util.hpp>
#include <D3D12/D3D12_CommandList.hpp>
#include <D3D12/D3D12_DescriptorHeapAllocator.hpp>
#include <D3D12/D3D12_Texture.hpp>

#include <Direct12XTK/Include/ResourceUploadBatch.h>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx12.h>
#include <ImGui/imgui_impl_win32.h>

#include <Assimp/color4.h>
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <Assimp/scene.h>


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
		Assimp::Importer importer;
		try {
			m_Width = width;
			m_Height = height;
			m_aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

#if defined(_DEBUG)
			GetDebugInterface();
#endif
			GetAdapters(false);
			CreateDevice(m_Adapter.Get());

			m_GraphicsCommandList = make_unique<GraphicsCommandList>(m_Device.Get());
			m_MainRTVDescriptorHeap = make_unique<DescriptorHeapAllocator>(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, SWAPCHAIN_BUFFER_COUNT);
			m_ResourceDescriptorHeap = make_unique<DescriptorHeapAllocator>(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 5);
			m_ImGuiDescriptorHeap = make_unique<DescriptorHeapAllocator>(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 3);
			m_CbMatrix = make_unique<ConstantBuffer<CB_Matrix>>(m_Device.Get());
			m_CbTest = make_unique<ConstantBuffer<CB_Test>>(m_Device.Get());
			
			CreateCommandQueue();
			CreateSwapChain(hwnd);
			CreateMainRTV();
			CreateRootSignature();
			LoadAssets();
			CreateResourceView();
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
		m_GraphicsCommandList->SetRenderTarget(1, &m_MainRTVDescriptorHeap->GetCpuHandle(m_FrameIndex));
		m_GraphicsCommandList->ClearRenderTarget(m_MainRTVDescriptorHeap->GetCpuHandle(m_FrameIndex), m_ClearColor);

		ID3D12DescriptorHeap* ppHeaps[] = { m_ResourceDescriptorHeap->GetDescriptorHeap() };
		m_GraphicsCommandList->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		m_GraphicsCommandList->GetCommandList()->SetGraphicsRootSignature(m_RootSignature.Get());
		m_GraphicsCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(0, m_ResourceDescriptorHeap->GetGpuHandle(0));
		m_GraphicsCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(1, m_ResourceDescriptorHeap->GetGpuHandle(1));
		m_GraphicsCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(2, m_ResourceDescriptorHeap->GetGpuHandle(2));

		Pass_Gui();
		RenderEnd();
		PresentSwapChain(true);
		MoveToNextFrame();
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
		createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
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
			m_Device.Get(),
			SWAPCHAIN_BUFFER_COUNT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			m_ImGuiDescriptorHeap->GetDescriptorHeap(),
			m_ImGuiDescriptorHeap->GetCpuHandle(),
			m_ImGuiDescriptorHeap->GetGpuHandle());
	}

	void Renderer::CreateDevice(IDXGIAdapter4* adapter)
	{
		ComPtr<ID3D12Device2> d3d12Device2;
		ThrowIfFailed(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_Device.GetAddressOf())));

#if defined(_DEBUG)
		ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(m_Device.As(&pInfoQueue)))
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

		ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));
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
			m_CommandQueue.Get(),
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
		for (UINT n = 0; n < SWAPCHAIN_BUFFER_COUNT; n++)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
			m_Device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, m_MainRTVDescriptorHeap->GetCpuHandle(n));
		}
	}

	void Renderer::LoadAssets() {

		DirectX::ResourceUploadBatch uploadBatch(m_Device.Get());
		uploadBatch.Begin();

		string textureDir = LADYBUG3D_RESOURCE_PATH;
		for (auto& resource : filesystem::recursive_directory_iterator(LADYBUG3D_RESOURCE_PATH)) {
			if (resource.path().extension() == ".png") {
				cout << "Find Texture At " << resource << endl;
			}
		}

		textureDir += "/Texture/Sample.png";
		wstring wstr(textureDir.begin(), textureDir.end());
		m_SampleTexture = make_unique<Texture>();
		m_SampleTexture->InitializeWICTexture(wstr.c_str(), uploadBatch, m_Device.Get());
		auto finish = uploadBatch.End(m_CommandQueue.Get());
		finish.wait();


		m_GraphicsCommandList->Begin();
		m_GraphicsCommandList->Close();
		ID3D12CommandList* ppCommandLists1[] = { m_GraphicsCommandList->GetCommandList() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists1), ppCommandLists1);
		MoveToNextFrame();

		m_GraphicsCommandList->Begin();

		static Model s_Model;
		for (auto& resource : filesystem::recursive_directory_iterator(LADYBUG3D_RESOURCE_PATH)) {
			if (resource.path().extension() == ".obj") {
				cout << "Find Obj Model At " << resource.path().string() << endl;
				
				m_Models.emplace_back(
					LoadModel(resource.path().string(), m_Device.Get(), m_GraphicsCommandList->GetCommandList()));

				break;
			}
		}
		m_GraphicsCommandList->Close();
		ID3D12CommandList* ppCommandLists[] = { m_GraphicsCommandList->GetCommandList() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		MoveToNextFrame();


		{
			ComPtr<ID3DBlob> vertexShader;
			ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
			UINT compileFlags = 0;
#endif

			for (auto& resource : filesystem::recursive_directory_iterator(LADYBUG3D_RESOURCE_PATH)) {
				if (resource.path().extension() == ".hlsl") {
					cout << "Find Shader At " << resource << endl;
					ThrowIfFailed(D3DCompileFromFile(resource.path().c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
					ThrowIfFailed(D3DCompileFromFile(resource.path().c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));
				}
			}

			// Define the vertex input layout.
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXTURECOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			// Describe and create the graphics pipeline state object (PSO).
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			psoDesc.pRootSignature = m_RootSignature.Get();
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.StencilEnable = FALSE;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;
			ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
		}

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

		ThrowIfFailed(m_CommandQueue->Signal(fence, fenceValue));
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
	
	void Renderer::Update()
	{
		m_CbTest->Data->index++;
	}

	void Renderer::RenderBegin()
	{
		m_FrameIndex = m_swapChain->GetCurrentBackBufferIndex();

		m_GraphicsCommandList->Begin();
		m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	}

	void Renderer::RenderEnd()
	{
		m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		m_GraphicsCommandList->Close();

		ID3D12CommandList* ppCommandLists[] = { m_GraphicsCommandList->GetCommandList() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
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
			ImGui::Image((ImTextureID)(m_ImGuiDescriptorHeap->GetGpuHandle(1).ptr), ImVec2(100, 100));
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

		
		ID3D12DescriptorHeap* heaps[] = { m_ImGuiDescriptorHeap->GetDescriptorHeap() };
		m_GraphicsCommandList->GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
		
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_GraphicsCommandList->GetCommandList());
	}

	void Renderer::CreateResourceView()
	{
		m_CbMatrix->CreateConstantBufferView(m_Device.Get(), m_ResourceDescriptorHeap->GetCpuHandle());
		m_CbTest->CreateConstantBufferView(m_Device.Get(), m_ResourceDescriptorHeap->GetCpuHandle(1));
		m_SampleTexture->CreateShaderResourceView(m_Device.Get(), m_ResourceDescriptorHeap->GetCpuHandle(2));

		m_SampleTexture->CreateShaderResourceView(m_Device.Get(), m_ImGuiDescriptorHeap->GetCpuHandle(1));
	}

	void Renderer::CreateRootSignature()
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = { D3D_ROOT_SIGNATURE_VERSION_1_1 };
		if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
		ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

		CD3DX12_ROOT_PARAMETER1 rootParameters[3];
		rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);
		rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc[2];
		samplerDesc[0].Init(0, D3D12_FILTER_ANISOTROPIC);
		samplerDesc[1].Init(1, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR);

		auto rootSignatureFlag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, _countof(samplerDesc), samplerDesc, rootSignatureFlag);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
		ThrowIfFailed(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
	}

}


