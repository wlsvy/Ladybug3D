#include "Editor.hpp"
#include "Scene.hpp"
#include "Util.hpp"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx12.h>
#include <ImGui/imgui_impl_win32.h>

#include <dxgi1_6.h>

#include <D3D12/D3D12_DescriptorHeapAllocator.hpp>


using namespace Ladybug3D::D3D12;

namespace Ladybug3D::Editor {

	void InitImGui(void* hwnd, ID3D12Device* device, UINT frameCount, DescriptorHeapAllocator* descriptor)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(
			device,
			frameCount,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			descriptor->GetDescriptorHeap(),
			descriptor->GetCpuHandle(),
			descriptor->GetGpuHandle());
	}

	void NewFrame()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void Render(ID3D12GraphicsCommandList* cmdList)
	{
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
	}

	void DrawSceneGraph()
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
		//ImGui::SameLine();

		//ImGui::BeginChild("Editor##Inspector", ImVec2(0, 0), true);
		//ImGui::Text("Inspector");
		//ImGui::Separator();
		//ImGui::Spacing();


		///*if (auto selected = scene.GetGuiSelected().lock())
		//{
		//	selected->GetGameObject()->OnGui();
		//}*/

		//ImGui::EndChild();
		ImGui::End();
	}

	void ShutDownImGui()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}