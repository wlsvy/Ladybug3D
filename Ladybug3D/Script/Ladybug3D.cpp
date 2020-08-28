#include "Ladybug3D.hpp"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx12.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Util.hpp>
#include <D3D12/D3D12_GpuInterface.hpp>
#include <WindowContainer.hpp>
#include <filesystem>

using namespace std;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
    Ladybug3D::Util::PrintHello();
    
    Ladybug3D::WindowContainer wc;
    Ladybug3D::WindowContainer::s_OnWndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
    };
    wc.Create("Hellow Ladybug3D", "Ladybug3D", 1280, 800);
    wc.Show();

    Ladybug3D::D3D12::GpuInterface gi;
    if (!gi.Initialize(wc.GetHandle(), 1280, 800)) {
        wc.Destroy();
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(wc.GetHandle());
    ImGui_ImplDX12_Init(
        gi.GetDevice(), 
        Ladybug3D::D3D12::FRAME_COUNT,
        DXGI_FORMAT_R8G8B8A8_UNORM, 
        gi.GetSrvDescriptorHeap(),
        gi.GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
        gi.GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (wc.Tick()) {
        // Start the Dear ImGui frame
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

        // Rendering
        gi.RenderBegin();
        gi.SetRenderTarget(1, gi.GetRtvDescriptorHeap(), false, nullptr);
        gi.ClearRenderTarget(gi.GetRtvDescriptorHeap(), (float*)&clear_color);
        gi.GetCommandList()->SetDescriptorHeaps(1, gi.GetSrvDescriptorHeapAddr());
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), gi.GetCommandList());
        gi.RenderEnd();
        gi.PresentSwapChain();
    }

    // Cleanup
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    wc.Destroy();

    return 0;
}