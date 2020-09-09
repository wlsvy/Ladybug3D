#include "Editor.hpp"

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
	void DrawSceneGraph()
	{
		/*static bool show_demo_window = true;

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		if (ImGui::Begin("Another Window"))
		{
			ImGui::Text("Hello from another window!");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Image((ImTextureID)(m_ImGuiDescriptorHeap->GetGpuHandle(1).ptr), ImVec2(100, 100));

			ImGui::DragFloat4("Clear Color", m_ClearColor, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::Text("Camera Transform");
			m_MainCam->GetTransform()->OnImGui();

			ImGui::End();
		}*/
	}
	void ShutDownImGui()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}