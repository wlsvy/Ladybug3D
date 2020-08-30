#include "Ladybug3D.hpp"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx12.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Util.hpp>
#include <WindowContainer.hpp>
#include <Renderer/Renderer.hpp>

using namespace std;
using namespace Ladybug3D;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct alignas(32) Foo {
    float a;
};

int main()
{
    Util::PrintHello();
    cout << "size of 32byte aligned Foo : " << sizeof(Foo) << endl;
    auto& windowContainer =  WindowContainer::GetInstance();
    windowContainer.Create("Hellow Ladybug3D", "Ladybug3D", 1280, 800);
    windowContainer.Show();

    Renderer::Renderer renderer;
    if (!renderer.Initialize(windowContainer.GetHandle(), 1280, 800)) {
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
        renderer.Render();
    }
   
    renderer.ShutDown();
    windowContainer.Destroy();
    return 0;
}