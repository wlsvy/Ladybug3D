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
#include "RendererDefine.hpp"

#include <iostream>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>

#include <D3D12/D3D12_Util.hpp>
#include <D3D12/D3D12_CommandList.hpp>
#include <D3D12/D3D12_DescriptorHeapAllocator.hpp>
#include <D3D12/D3D12_Texture.hpp>
#include <D3D12/D3D12_ConstantBuffer.hpp>
#include <D3D12/D3D12_PipelineState.hpp>

#include <Direct12XTK/Include/ResourceUploadBatch.h>


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

			if (!DX12Resources::Initialize(hwnd, width, height)) {
				return false;
			}

			m_ResourceDescriptorHeap = make_unique<DescriptorHeapAllocator>(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, ResourceDescriptorIndex::Max);

			m_CB_PerObject = make_unique<ConstantBuffer<CB_PerObject>>(m_Device.Get(), MAX_OBJECT_COUNT);
			m_CB_PerScene = make_unique<ConstantBuffer<CB_PerScene>>(m_Device.Get());

			

			LoadAssets();
			CreatePipelineState();
			CreateResourceView();

			m_CurrentScene = make_shared<Scene>();
			m_CurrentScene->Initialize();
			m_Editor = make_unique<Editor>();
			m_Editor->Initialize(hwnd, m_Device.Get(), SWAPCHAIN_BUFFER_COUNT);
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
		m_GraphicsCommandList->Reset();

		resourceManager.Initialize();

		m_GraphicsCommandList->Close();

		ID3D12CommandList* ppCommandLists[] = { m_GraphicsCommandList->GetCommandList() };
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		WaitForPreviousFrame();
	}

	void Renderer::CreatePipelineState()
	{
		HRESULT hr;
		auto& resourceManager = ResourceManager::GetInstance();

		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = { D3D_ROOT_SIGNATURE_VERSION_1_1 };
		if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	//b0
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);	//b1
		ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0);	//t0 ~ t1

		CD3DX12_ROOT_PARAMETER1 rootParameters[3];
		rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc[6];
		samplerDesc[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);			//s0 : pointClamp
		samplerDesc[1].Init(1, D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);	//s1 : linearClamp
		samplerDesc[2].Init(2, D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);		//s2 : linearWrap
		samplerDesc[3].Init(3, D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR);	//s3 : linearMirror
		samplerDesc[4].Init(4, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR);		//s4 : trilinearMirror
		samplerDesc[5].Init(5, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR);				//s5 : anisotropicMirror

		auto rootSignatureFlag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, _countof(samplerDesc), samplerDesc, rootSignatureFlag);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ComPtr<ID3D12RootSignature> rootSignature;
		hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
		if (FAILED(hr)) {
			cout << "Failed To Serialize RootSignature - " <<  static_cast<const char*>(error->GetBufferPointer()) << endl;
			ThrowIfFailed(hr);
		}
		ThrowIfFailed(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)),
			"Failed To Create RootSignature");

		UINT compileFlags = 0;
#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		D3D12_INPUT_ELEMENT_DESC inputElement_Vertex3D[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		D3D12_INPUT_ELEMENT_DESC inputElement_Vertex_Color[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		auto depthStencilDesc_None = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		depthStencilDesc_None.DepthEnable = FALSE;
		depthStencilDesc_None.StencilEnable = FALSE;
		depthStencilDesc_None.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc_None.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		auto depthStencilDesc_DepthTestEnable = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		depthStencilDesc_DepthTestEnable.DepthEnable = TRUE;
		depthStencilDesc_DepthTestEnable.StencilEnable = FALSE;
		depthStencilDesc_DepthTestEnable.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc_DepthTestEnable.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		auto depthStencilDesc_DepthStencilTestEnable = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		depthStencilDesc_DepthStencilTestEnable.DepthEnable = TRUE;
		depthStencilDesc_DepthStencilTestEnable.StencilEnable = TRUE;
		depthStencilDesc_DepthStencilTestEnable.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		auto rasterizerDesc_CullBack = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		rasterizerDesc_CullBack.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc_CullBack.CullMode = D3D12_CULL_MODE_BACK;

		auto rasterizerDesc_CullNone = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		rasterizerDesc_CullNone.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc_CullNone.CullMode = D3D12_CULL_MODE_NONE;

		auto rasterizerDesc_WireFrame = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		rasterizerDesc_WireFrame.FillMode = D3D12_FILL_MODE_WIREFRAME;
		rasterizerDesc_WireFrame.CullMode = D3D12_CULL_MODE_NONE;


		auto blendStateDesc_Opaque = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		blendStateDesc_Opaque.IndependentBlendEnable = false;
		for (auto& renderTargetDesc : blendStateDesc_Opaque.RenderTarget) {
			renderTargetDesc.BlendEnable = false;
			renderTargetDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			renderTargetDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			renderTargetDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
			renderTargetDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		}

		auto blendStateDesc_Alpha = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		blendStateDesc_Alpha.IndependentBlendEnable = true;
		for (auto& renderTargetDesc : blendStateDesc_Alpha.RenderTarget) {
			renderTargetDesc.BlendEnable = true;
			renderTargetDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			renderTargetDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			renderTargetDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
			renderTargetDesc.DestBlendAlpha = D3D12_BLEND_ONE;
		}

		auto blendStateDesc_Additive = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		blendStateDesc_Additive.IndependentBlendEnable = true;
		for (auto& renderTargetDesc : blendStateDesc_Additive.RenderTarget) {
			renderTargetDesc.BlendEnable = true;
			renderTargetDesc.SrcBlend = D3D12_BLEND_ONE;
			renderTargetDesc.DestBlend = D3D12_BLEND_ONE;
			renderTargetDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
			renderTargetDesc.DestBlendAlpha = D3D12_BLEND_ONE;
		}

		{
			ComPtr<ID3DBlob> vertexShader;
			ComPtr<ID3DBlob> pixelShader;
			ComPtr<ID3DBlob> errorMsg;
			auto ShaderPath = resourceManager.GetShaderPath("shaders");

			hr = D3DCompileFromFile(ShaderPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &errorMsg);
			if (FAILED(hr)) {
				cout << "Failed To Compile Shader - " << ShaderPath << " : " << static_cast<const char*>(errorMsg->GetBufferPointer()) << endl;
				ThrowIfFailed(hr);
			}
			hr = D3DCompileFromFile(ShaderPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &errorMsg);
			if (FAILED(hr)) {
				cout << "Failed To Compile Shader - " << ShaderPath << " : " << static_cast<const char*>(errorMsg->GetBufferPointer()) << endl;
				ThrowIfFailed(hr);
			}

			
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { inputElement_Vertex3D, _countof(inputElement_Vertex3D) };
			psoDesc.pRootSignature = rootSignature.Get();
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
			psoDesc.RasterizerState = rasterizerDesc_CullBack;
			psoDesc.BlendState = blendStateDesc_Opaque;
			psoDesc.DepthStencilState = depthStencilDesc_DepthTestEnable;
			psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;

			m_PSO_Default = make_unique<PipelineState>(m_Device.Get(), &psoDesc);
		}
		
		{
			ComPtr<ID3DBlob> vertexShader;
			ComPtr<ID3DBlob> pixelShader;
			ComPtr<ID3DBlob> errorMsg;
			auto ShaderPath = resourceManager.GetShaderPath("Skybox");

			hr = D3DCompileFromFile(ShaderPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &errorMsg);
			if (FAILED(hr)) {
				cout << "Failed To Compile Shader - " << ShaderPath << " : " << static_cast<const char*>(errorMsg->GetBufferPointer()) << endl;
				ThrowIfFailed(hr);
			}
			hr = D3DCompileFromFile(ShaderPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &errorMsg);
			if (FAILED(hr)) {
				cout << "Failed To Compile Shader - " << ShaderPath << " : " << static_cast<const char*>(errorMsg->GetBufferPointer()) << endl;
				ThrowIfFailed(hr);
			}

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { inputElement_Vertex3D, _countof(inputElement_Vertex3D) };
			psoDesc.pRootSignature = rootSignature.Get();
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
			psoDesc.RasterizerState = rasterizerDesc_CullNone;
			psoDesc.BlendState = blendStateDesc_Opaque;
			psoDesc.DepthStencilState = depthStencilDesc_DepthTestEnable;
			psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;

			m_PSO_Skybox = make_unique<PipelineState>(m_Device.Get(), &psoDesc);
		}
	}

	void Renderer::OnUpdate()
	{
		m_CurrentScene->OnUpdate();
		m_MainCam->OnUpdate();
		m_MainCam->UpdateView();

		UpdateConstantBuffer();
	}

	void Renderer::OnRender()
	{
		RenderBegin();
		Pass_Skybox();
		Pass_Main();
		Pass_Editor();
		RenderEnd();

		ThrowIfFailed(m_swapChain->Present(1, 0));
		WaitForPreviousFrame();
	}

	void Renderer::OnDestroy()
	{
		cout << "Shut Down Renderer ..." << endl;
		WaitForPreviousFrame();

		m_CurrentScene->OnDestroy();
		m_Editor->ShutDownImGui();
		ResourceManager::GetInstance().Destroy();
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
		m_CB_PerScene->CreateConstantBufferView(m_Device.Get(), m_ResourceDescriptorHeap->GetCpuHandle(ResourceDescriptorIndex::CB_PerScene));

		for (UINT i = 0; i < MAX_OBJECT_COUNT; i++) {
			m_CB_PerObject->CreateConstantBufferView(m_Device.Get(), m_ResourceDescriptorHeap->GetCpuHandle(ResourceDescriptorIndex::CB_PerObject + i), i);
		}
		
		//m_DepthStencilTextures[0]->CreateShaderResourceView(m_Device.Get(), m_ResourceDescriptorHeap->GetCpuHandle(ResourceDescriptorIndex::SRV_SampleTexture));
		ResourceManager::GetInstance().GetTexture("Sample")->CreateShaderResourceView(m_Device.Get(), m_ResourceDescriptorHeap->GetCpuHandle(ResourceDescriptorIndex::SRV_SampleTexture));
		ResourceManager::GetInstance().GetTexture("grasscube1024")->CreateCubeMapShaderResourceView(m_Device.Get(), m_ResourceDescriptorHeap->GetCpuHandle(ResourceDescriptorIndex::SRV_Skybox));

	}

	void Renderer::UpdateConstantBuffer()
	{
		m_CB_PerScene->Data->viewMatrix = m_MainCam->GetViewMatrix();
		m_CB_PerScene->Data->projMatrix = m_MainCam->GetProjectionMatrix();
		m_CB_PerScene->Data->viewProjMatrix = m_MainCam->GetViewProjectionMatrix();
		m_CB_PerScene->Data->CameraWorldPosition = m_MainCam->GetTransform()->positionVec;
		m_CB_PerScene->Data->CameraWorldMatrix = XMMatrixTranslationFromVector(m_MainCam->GetTransform()->positionVec);

		auto& sceneObjects = m_CurrentScene->GetSceneObjects();
		UINT index = 0;

		for (auto& obj : sceneObjects) {
			for (auto& mesh : obj->Model->GetMeshes()) {

				auto& gpuObjData = m_CB_PerObject->Data[index++];

				gpuObjData.worldMatrix = mesh.GetWorldMatrix() * obj->GetTransform()->GetWorldMatrix();
				gpuObjData.prevWvpWorld = gpuObjData.curWvpMatrix;
				gpuObjData.curWvpMatrix = gpuObjData.worldMatrix * m_MainCam->GetViewProjectionMatrix();
			}
		}
	}

	void Renderer::DrawMesh()
	{
	}

	void Renderer::DrawScreenQuad()
	{
		static D3D12_VERTEX_BUFFER_VIEW screenVertexBufferView = {0, 0, 0};
		static D3D12_INDEX_BUFFER_VIEW screenIndexBufferView = { 0, 0, DXGI_FORMAT_R32_UINT };

		m_GraphicsCommandList->GetCommandList()->IASetVertexBuffers(0, 0, &screenVertexBufferView);
		m_GraphicsCommandList->GetCommandList()->IASetIndexBuffer(&screenIndexBufferView);
		m_GraphicsCommandList->GetCommandList()->DrawInstanced(4, 1, 0, 0);
	}

	void Renderer::RenderBegin()
	{
		m_FrameIndex = m_swapChain->GetCurrentBackBufferIndex();

		m_GraphicsCommandList->Reset();
		m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		m_GraphicsCommandList->ClearRenderTarget(m_MainRTVDescriptorHeap->GetCpuHandle(m_FrameIndex), m_ClearColor);
		m_GraphicsCommandList->GetCommandList()->ClearDepthStencilView(m_DSVDescriptorHeap->GetCpuHandle(m_FrameIndex), D3D12_CLEAR_FLAG_DEPTH, 1.0f ,0, 0, nullptr);
		m_GraphicsCommandList->GetCommandList()->RSSetViewports(1, &m_viewport);
		m_GraphicsCommandList->GetCommandList()->RSSetScissorRects(1, &m_scissorRect);

		ID3D12DescriptorHeap* ppHeaps[] = { m_ResourceDescriptorHeap->GetDescriptorHeap() };
		m_GraphicsCommandList->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
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
		m_GraphicsCommandList->SetPipelineState(m_PSO_Default.get());
		m_GraphicsCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(RootSignatureIndex::CB_PerScene, m_ResourceDescriptorHeap->GetGpuHandle(ResourceDescriptorIndex::CB_PerScene));
		m_GraphicsCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(RootSignatureIndex::SRV_Texture, m_ResourceDescriptorHeap->GetGpuHandle(ResourceDescriptorIndex::SRV_SampleTexture));
		m_GraphicsCommandList->SetRenderTarget(1, &m_MainRTVDescriptorHeap->GetCpuHandle(m_FrameIndex), &m_DSVDescriptorHeap->GetCpuHandle(m_FrameIndex));
		
		auto& sceneObjects = m_CurrentScene->GetSceneObjects();
		UINT index = 0;

		for (auto& obj : sceneObjects) {
			for (auto& mesh : obj->Model->GetMeshes()) {
				m_GraphicsCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(1, m_ResourceDescriptorHeap->GetGpuHandle(ResourceDescriptorIndex::CB_PerObject + index++));
				m_GraphicsCommandList->GetCommandList()->IASetVertexBuffers(0, 1, mesh.GetVertexBufferView());
				m_GraphicsCommandList->GetCommandList()->IASetIndexBuffer(mesh.GetIndexBufferView());
				m_GraphicsCommandList->GetCommandList()->DrawIndexedInstanced(mesh.GetIndexBuffer()->GetNumIndices(), 1, 0, 0, 0);
			}
		}
	}

	void Renderer::Pass_Editor()
	{
		ID3D12DescriptorHeap* heaps[] = { m_Editor->GetDescriptorHeap() };
		m_GraphicsCommandList->GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
		m_GraphicsCommandList->SetRenderTarget(1, &m_MainRTVDescriptorHeap->GetCpuHandle(m_FrameIndex));

		m_Editor->NewFrame();
		m_Editor->DrawSampleWindow();
		m_Editor->DrawSceneGraph();
		m_Editor->Render(m_GraphicsCommandList->GetCommandList());
	}

	void Renderer::Pass_Skybox()
	{
		static auto skyboxModel = ResourceManager::GetInstance().GetModel("sphere").get();

		m_GraphicsCommandList->SetPipelineState(m_PSO_Skybox.get());
		m_GraphicsCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(RootSignatureIndex::SRV_Texture, m_ResourceDescriptorHeap->GetGpuHandle(ResourceDescriptorIndex::SRV_SampleTexture));
		m_GraphicsCommandList->SetRenderTarget(1, &m_MainRTVDescriptorHeap->GetCpuHandle(m_FrameIndex), &m_DSVDescriptorHeap->GetCpuHandle(m_FrameIndex));

		for (auto& mesh : skyboxModel->GetMeshes()) {
			m_GraphicsCommandList->GetCommandList()->IASetVertexBuffers(0, 1, mesh.GetVertexBufferView());
			m_GraphicsCommandList->GetCommandList()->IASetIndexBuffer(mesh.GetIndexBufferView());
			m_GraphicsCommandList->GetCommandList()->DrawIndexedInstanced(mesh.GetIndexBuffer()->GetNumIndices(), 1, 0, 0, 0);
			m_GraphicsCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(1, m_ResourceDescriptorHeap->GetGpuHandle(ResourceDescriptorIndex::CB_PerObject + 10));
		}
	}
}