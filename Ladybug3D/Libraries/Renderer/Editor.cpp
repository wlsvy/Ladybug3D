#include "Editor.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Transform.hpp"
#include "Util.hpp"
#include "ResourceManager.hpp"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx12.h>
#include <ImGui/imgui_impl_win32.h>

#include <dxgi1_6.h>

#include <D3D12/D3D12_DescriptorHeapAllocator.hpp>
#include <D3D12/D3D12_Texture.hpp>


using namespace std;
using namespace Ladybug3D::D3D12;

namespace Ladybug3D {

	void Editor::Initialize(void* hwnd, ID3D12Device* device, UINT frameCount)
	{
		m_DescriptorHeap = make_unique<DescriptorHeapAllocator>(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, EDITOR_DESCRIPTOR_SIZE);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(
			device,
			frameCount,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			m_DescriptorHeap->GetDescriptorHeap(),
			m_DescriptorHeap->GetCpuHandle(),
			m_DescriptorHeap->GetGpuHandle());

		ResourceManager::GetInstance().GetTexture("Sample")->CreateShaderResourceView(device, m_DescriptorHeap->GetCpuHandle(1));
	}

	void Editor::NewFrame()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void Editor::Render(ID3D12GraphicsCommandList* cmdList)
	{
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
	}

	void Editor::DrawSampleWindow()
	{
		static bool show_demo_window = true;

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		if (ImGui::Begin("Another Window"))
		{
			ImGui::Text("Hello from another window!");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Image((ImTextureID)(m_DescriptorHeap->GetGpuHandle(1).ptr), ImVec2(100, 100));
			ImGui::End();
		}
	}

	void Editor::DrawSceneGraph()
	{
		if (!ImGui::Begin("Scene Graph", nullptr, ImGuiWindowFlags_NoCollapse)) {
			ImGui::End();
			return;
		}

		ImGuiIO& io = ImGui::GetIO();

		ImGui::BeginChild("Editor##Hierarchy", ImVec2(io.DisplaySize.x * 0.15f, 0), true);
		ImGui::Text("Hierarchy");
		ImGui::Separator();
		ImGui::Spacing();

		auto scene = Util::GetCurrentScene();
		scene->OnImGui();

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginChild("Editor##Inspector", ImVec2(0, 0), true);
		ImGui::Text("Inspector");
		ImGui::Separator();
		ImGui::Spacing();


		if (auto selected = scene->GetGuiSelected().lock())
		{
			selected->GetSceneObject()->OnImGui();
		}

		ImGui::EndChild();
		ImGui::End();
	}

	void Editor::ShutDownImGui()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	ID3D12DescriptorHeap* Editor::GetDescriptorHeap()
	{
		return m_DescriptorHeap->GetDescriptorHeap();
	}
}