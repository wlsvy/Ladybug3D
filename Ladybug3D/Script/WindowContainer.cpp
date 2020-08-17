#include "WindowContainer.hpp"

namespace Ladybug3D {

    LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (WindowContainer::s_OnWndProc) {
            WindowContainer::s_OnWndProc(hwnd, msg, wParam, lParam);
        }
        return ::DefWindowProc(hwnd, msg, wParam, lParam);
    }

    std::function<void(HWND, UINT, WPARAM, LPARAM)> WindowContainer::s_OnWndProc;

    bool WindowContainer::Create(
        const char* title, 
        const char* className,
        UINT width,
        UINT height) 
    {
        m_WindowTitle = title;
        m_WindowClassName = className;
        m_Width = width;
        m_Height = height;

        // Register the Window Class
        WNDCLASSEX wc;
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = 0;
        wc.lpfnWndProc = WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = ::GetModuleHandle(nullptr);
        wc.hIcon = nullptr;
        wc.hCursor = nullptr;
        wc.hbrBackground = nullptr;
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = m_WindowClassName.c_str();
        wc.hIconSm = nullptr;

        if (!::RegisterClassEx(&wc))
        {
            ::MessageBox(nullptr, "Window registration failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
            return false;
        }

        m_Instance = wc.hInstance;

        // Create the Window
        m_Handle = ::CreateWindow
        (
            m_WindowClassName.c_str(),
            m_WindowTitle.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, m_Width, m_Height,
            nullptr, nullptr, m_Instance, nullptr
        );

        if (!m_Handle)
        {
            ::MessageBox(nullptr, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
            return false;
        }

        return true;
    }
    void WindowContainer::Show()
    {
        ::ShowWindow(m_Handle, SW_SHOWDEFAULT);
        ::UpdateWindow(m_Handle);
        ::SetFocus(m_Handle);
    }
    bool WindowContainer::Tick()
    {
        MSG msg;
        ::ZeroMemory(&msg, sizeof(msg));

        while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                return false;
        }

        return true;
    }
    void WindowContainer::Destroy()
    {
        ::DestroyWindow(m_Handle);
        ::UnregisterClass(m_WindowClassName.c_str(), m_Instance);
    }
}


