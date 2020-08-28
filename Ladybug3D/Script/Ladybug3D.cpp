#include "Ladybug3D.hpp"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx12.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Util.hpp>
#include <D3D12/D3D12_GpuInterface.hpp>
#include <WindowContainer.hpp>
#include <Renderer/Renderer.hpp>

using namespace std;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool g_FullScreenSignal = false;
UINT g_Width;
UINT g_Height;

int main()
{
    Ladybug3D::Util::PrintHello();
    
    Ladybug3D::WindowContainer windowContainer;
    windowContainer.Create("Hellow Ladybug3D", "Ladybug3D", 1280, 800);
    windowContainer.Show();

    Ladybug3D::Renderer::Renderer renderer;
    if (!renderer.Initialize(windowContainer.GetHandle(), 1280, 800)) {
        windowContainer.Destroy();
        return 1;
    }

    Ladybug3D::WindowContainer::s_OnWndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
    };
    Ladybug3D::WindowContainer::s_OnResize = [&renderer](UINT width, UINT height)
    {
        g_Width = width;
        g_Height = height;
        g_FullScreenSignal = true;
    };

    while (windowContainer.Tick()) {
        if (g_FullScreenSignal) {
            g_FullScreenSignal = false;
            renderer.ResizeSwapChainBuffer(g_Width, g_Height);
        }
        renderer.Render();
    }
   
    renderer.ShutDown();
    windowContainer.Destroy();
    return 0;
}