#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Camera.hpp"
#include "Transform.hpp"
#include "ConstantBufferType.hpp"
#include "Editor.hpp"
#include "ResourceManager.hpp"

#include <filesystem>
#include <iostream>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <ImGui/imgui.h>
#include <Direct12XTK/Include/ResourceUploadBatch.h>

#include <D3D12/D3D12_Util.hpp>
#include <D3D12/D3D12_CommandList.hpp>
#include <D3D12/D3D12_DescriptorHeapAllocator.hpp>
#include <D3D12/D3D12_Texture.hpp>
#include <D3D12/D3D12_VertexType.hpp>
#include <D3D12/D3D12_ConstantBuffer.hpp>
#include <D3D12/D3D12_VertexBuffer.hpp>
#include <D3D12/D3D12_IndexBuffer.hpp>


using namespace std;
using namespace DirectX;
using namespace Ladybug3D::D3D12;
using namespace Microsoft::WRL;

namespace Ladybug3D {

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::OnInit(HWND hwnd, UINT width, UINT height)
	{
		try {
			cout << "Initialize Renderer ..." << endl;

			if (!D3D12Resources::Initialize(hwnd, width, height)) {
				return false;
			}

			m_GraphicsCommandList = make_unique<GraphicsCommandList>(m_Device.Get());
			m_ResourceDescriptorHeap = make_unique<DescriptorHeapAllocator>(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 128);
			m_ImGuiDescriptorHeap = make_unique<DescriptorHeapAllocator>(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 3);

			m_CB_PerObject = make_unique<ConstantBuffer<CB_PerObject>>(m_Device.Get(), MAX_OBJECT_COUNT);
			m_CB_PerScene = make_unique<ConstantBuffer<CB_PerScene>>(m_Device.Get());

			CreateRootSignature();
			LoadAssets();
			CreateResourceView();
			Editor::InitImGui(hwnd, m_Device.Get(), SWAPCHAIN_BUFFER_COUNT, m_ImGuiDescriptorHeap.get());

			m_CurrentScene = make_shared<Scene>();
			m_CurrentScene->Initialize();
			m_MainCam = make_shared<Camera>();
			m_MainCam->SetProjectionValues(90.0f, m_aspectRatio, 0.1f, 1000.0f);
		}
		catch (exception& e) {
			cout << e.what() << endl;
			return false;
		}
		return true;
	}

	void Renderer::LoadAssets()
	{
		auto& resourceManager = ResourceManager::GetInstance();
		// Create the pipeline state, which includes compiling and loading shaders.
		{
			ComPtr<ID3DBlob> vertexShader;
			ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
			// Enable better shader debugging with the graphics debugging tools.
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
			UINT compileFlags = 0;
#endif

			for (auto& resource : filesystem::recursive_directory_iterator(LADYBUG3D_RESOURCE_PATH)) {
				if (resource.path().extension() == L".hlsl"){
					if (resource.path().stem() != L"shaders") {
						//continue;
					}
					cout << "Find Shader At " << resource << endl;
					ThrowIfFailed(D3DCompileFromFile(resource.path().c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr),
						"Failed To Compile Vertex Shader");
					ThrowIfFailed(D3DCompileFromFile(resource.path().c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr),
						"Failed To Compile Pixel Shader");
				}
			}

			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TANGENT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			psoDesc.pRootSignature = m_rootSignature.Get();
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
			ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)),
				"Failed To Create Pipeline State Object");
		}

		


		// Load Model
		{
			m_GraphicsCommandList->Begin();

			resourceManager.Initialize();

			m_GraphicsCommandList->Close();

			ID3D12CommandList* ppCommandLists[] = { m_GraphicsCommandList->GetCommandList() };
			m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
			WaitForPreviousFrame();
		}
	}

	void Renderer::OnUpdate()
	{
		m_CurrentScene->OnUpdate();
		m_MainCam->OnUpdate();
		m_MainCam->UpdateView();
	}

	void Renderer::OnRender()
	{
		RenderBegin();
		Pass_Main();
		Pass_ImGui();
		RenderEnd();

		ThrowIfFailed(m_swapChain->Present(1, 0));
		WaitForPreviousFrame();
	}

	void Renderer::OnDestroy()
	{
		cout << "Shut Down Renderer ..." << endl;
		WaitForPreviousFrame();
		m_CurrentScene->OnDestroy();

		Editor::ShutDownImGui();
	}

	void Renderer::WaitForPreviousFrame()
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

	void Renderer::CreateResourceView()
	{
		//CBV, SRV
		m_CB_PerScene->CreateConstantBufferView(m_Device.Get(), m_ResourceDescriptorHeap->GetCpuHandle());

		for (UINT i = 0; i < MAX_OBJECT_COUNT; i++) {
			m_CB_PerObject->CreateConstantBufferView(m_Device.Get(), m_ResourceDescriptorHeap->GetCpuHandle(i + 1), i);
		}

		//Imgui SRV
		ResourceManager::GetInstance().GetTexture("Sample")->CreateShaderResourceView(m_Device.Get(), m_ImGuiDescriptorHeap->GetCpuHandle(1));
	}

	void Renderer::CreateRootSignature()
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = { D3D_ROOT_SIGNATURE_VERSION_1_1 };
		if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	//b0
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);	//b1

		CD3DX12_ROOT_PARAMETER1 rootParameters[2];
		rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_ALL);

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
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error),
			"Failed To Serialize RootSignature");
		ThrowIfFailed(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf())),
			"Failed To Create RootSignature");
	}

	void Renderer::RenderBegin()
	{
		m_FrameIndex = m_swapChain->GetCurrentBackBufferIndex();

		m_GraphicsCommandList->Begin();
		m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		m_GraphicsCommandList->ClearRenderTarget(m_MainRTVDescriptorHeap->GetCpuHandle(m_FrameIndex), m_ClearColor);
		m_GraphicsCommandList->GetCommandList()->RSSetViewports(1, &m_viewport);
		m_GraphicsCommandList->GetCommandList()->RSSetScissorRects(1, &m_scissorRect);
	}

	void Renderer::RenderEnd()
	{
		m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		m_GraphicsCommandList->Close();

		ID3D12CommandList* ppCommandLists[] = { m_GraphicsCommandList->GetCommandList() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	void Renderer::Pass_Main()
	{
		auto& sceneObjects = m_CurrentScene->GetSceneObjects();

		m_CB_PerScene->Data->viewMatrix = m_MainCam->GetViewMatrix();
		m_CB_PerScene->Data->projMatrix = m_MainCam->GetProjectionMatrix();
		m_CB_PerScene->Data->viewProjMatrix = m_MainCam->GetViewProjectionMatrix();

		ID3D12DescriptorHeap* ppHeaps[] = { m_ResourceDescriptorHeap->GetDescriptorHeap() };
		m_GraphicsCommandList->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		m_GraphicsCommandList->GetCommandList()->SetPipelineState(m_pipelineState.Get());
		m_GraphicsCommandList->GetCommandList()->SetGraphicsRootSignature(m_rootSignature.Get());
		m_GraphicsCommandList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_GraphicsCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(0, m_ResourceDescriptorHeap->GetGpuHandle(0));
		m_GraphicsCommandList->SetRenderTarget(1, &m_MainRTVDescriptorHeap->GetCpuHandle(m_FrameIndex));

		UINT index = 0;
		for (auto& obj : sceneObjects) {
			for (auto& mesh : obj->Model->GetMeshes()) {

				auto& gpuObjData = m_CB_PerObject->Data[index];

				gpuObjData.worldMatrix = mesh.GetWorldMatrix() * obj->GetTransform()->GetWorldMatrix();
				gpuObjData.prevWvpWorld = gpuObjData.curWvpMatrix;
				gpuObjData.curWvpMatrix = gpuObjData.worldMatrix * m_MainCam->GetViewProjectionMatrix();
				m_GraphicsCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(1, m_ResourceDescriptorHeap->GetGpuHandle(1 + index));
				index++;

				m_GraphicsCommandList->GetCommandList()->IASetVertexBuffers(0, 1, mesh.GetVertexBufferView());
				m_GraphicsCommandList->GetCommandList()->IASetIndexBuffer(mesh.GetIndexBufferView());
				m_GraphicsCommandList->GetCommandList()->DrawIndexedInstanced(mesh.GetIndexBuffer()->GetNumIndices(), 1, 0, 0, 0);
			}
		}
	}

	void Renderer::Pass_ImGui()
	{
		ID3D12DescriptorHeap* heaps[] = { m_ImGuiDescriptorHeap->GetDescriptorHeap() };
		m_GraphicsCommandList->GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
		m_GraphicsCommandList->SetRenderTarget(1, &m_MainRTVDescriptorHeap->GetCpuHandle(m_FrameIndex));

		Editor::NewFrame();

		static bool show_demo_window = true;

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		if (ImGui::Begin("Another Window"))
		{
			ImGui::Text("Hello from another window!");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Image((ImTextureID)(m_ImGuiDescriptorHeap->GetGpuHandle(1).ptr), ImVec2(100, 100));
			ImGui::DragFloat4("Clear Color", m_ClearColor, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::End();
		}

		Editor::DrawSceneGraph();
		Editor::Render(m_GraphicsCommandList->GetCommandList());
	}
}