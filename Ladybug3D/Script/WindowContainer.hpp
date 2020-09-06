#pragma once
#include <Windows.h>
#include <functional>
#include <string>
#include <Renderer/Singleton.hpp>

namespace Ladybug3D {
	
    class WindowContainer : public Singleton<WindowContainer> {
        friend LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    public:
        bool Create(
            const char* title,
            const char* className,
            UINT width,
            UINT height);
        void Show();
        bool Tick();
        void Destroy();

        HWND GetHandle() { return m_Handle; }
        HINSTANCE GetWindowInstance() { return m_Instance; }

        UINT GetWidth() const { return m_Width; }
        UINT GetHeight() const { return m_Height; }

        template<typename T>
        void SetWndProcCallback(T& callback) { m_OnWindowProcedure = callback; }
        template<typename T>
        void SetWndResizeCallback(T& callback) { m_OnWindowResize = callback; }

    private:
        void ToggleFullScreen();
        void ResizeWindow();

        HWND m_Handle;
        HINSTANCE m_Instance;

        std::string m_WindowTitle;
        std::string m_WindowClassName;
        UINT m_Width;
        UINT m_Height;

        RECT m_WindowRect;
        bool m_IsFullScreen = false;

        std::function<void(HWND, UINT, WPARAM, LPARAM)> m_OnWindowProcedure = [](HWND, UINT, WPARAM, LPARAM) {};
        std::function<void(UINT, UINT)> WindowContainer::m_OnWindowResize = [](UINT, UINT) {};
    };
}