#pragma once
#include <Windows.h>
#include <functional>
#include <string>

namespace Ladybug3D {
	
    class WindowContainer {
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
        HINSTANCE GetInstance() { return m_Instance; }

        UINT GetWidth() const { return m_Width; }
        UINT GetHeight() const { return m_Height; }

        static std::function<void(HWND, UINT, WPARAM, LPARAM)> s_OnWndProc;

    private:
        HWND m_Handle;
        HINSTANCE m_Instance;

        std::string m_WindowTitle;
        std::string m_WindowClassName;
        UINT m_Width;
        UINT m_Height;
    };
}