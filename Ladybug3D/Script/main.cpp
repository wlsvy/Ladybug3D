#include <iostream>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx12.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <WindowContainer.hpp>
#include <Renderer/Renderer.hpp>

#include <Assimp/color4.h>
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <Assimp/scene.h>

using namespace std;
using namespace Ladybug3D;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
    cout << "Hello Ladybug3D!!\n";

    auto& windowContainer =  WindowContainer::GetInstance();
    windowContainer.Create("Hellow Ladybug3D", "Ladybug3D", 1280, 800);
    windowContainer.Show();

    auto& renderer = Renderer::GetInstance();
    if (!renderer.OnInit(windowContainer.GetHandle(), 1280, 800)) {
        windowContainer.Destroy();
        return 1;
    }

    windowContainer.SetWndProcCallback([](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
    });
    windowContainer.SetWndResizeCallback([&renderer](UINT width, UINT height)
    {
        renderer.ResizeSwapChainBuffer(width, height);
    });

    while (windowContainer.Tick()) {
        renderer.OnUpdate();
        renderer.OnRender();
    }
   
    renderer.OnDestroy();
    windowContainer.Destroy();
    return 0;
}