#include "WindowContainer.hpp"

namespace Ladybug3D {

    LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        WindowContainer::GetInstance().m_OnWindowProcedure(hwnd, msg, wParam, lParam);
        switch (msg)
        {
        case WM_PAINT: break;
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case VK_ESCAPE: ::PostQuitMessage(0); break;
            case VK_RETURN: break;
            case VK_F11: WindowContainer::GetInstance().ToggleFullScreen();  break;
            }
            break;
        }
        case WM_SIZE: WindowContainer::GetInstance().ResizeWindow(); break;
        case WM_DESTROY: ::PostQuitMessage(0); break;
        }
        return ::DefWindowProc(hwnd, msg, wParam, lParam);
    }


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

        // Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
        // Using this awareness context allows the client area of the window 
        // to achieve 100% scaling while still allowing non-client window content to 
        // be rendered in a DPI sensitive fashion.
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

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
        ::GetWindowRect(m_Handle, &m_WindowRect);
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

            if (msg.message == WM_QUIT) {
                return false;
            }
        }
        return true;
    }
    void WindowContainer::Destroy()
    {
        m_OnWindowProcedure = [](HWND, UINT, WPARAM, LPARAM) {};
        m_OnWindowResize = [](UINT, UINT) {};
        ::DestroyWindow(m_Handle);
        ::UnregisterClass(m_WindowClassName.c_str(), m_Instance);
    }

    void WindowContainer::ToggleFullScreen()
    {
        m_IsFullScreen = !m_IsFullScreen;

        // Switching to isFullscreen.
        if (m_IsFullScreen) 
        {
            ::GetWindowRect(m_Handle, &m_WindowRect);

            UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

            ::SetWindowLongW(m_Handle, GWL_STYLE, windowStyle);

            HMONITOR hMonitor = ::MonitorFromWindow(m_Handle, MONITOR_DEFAULTTONEAREST);
            MONITORINFOEX monitorInfo = {};
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            ::GetMonitorInfo(hMonitor, &monitorInfo);

            ::SetWindowPos(m_Handle, HWND_TOP,
                monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE);

            ::ShowWindow(m_Handle, SW_MAXIMIZE);
        }
        else
        {
            // Restore all the window decorators.
            ::SetWindowLong(m_Handle, GWL_STYLE, WS_OVERLAPPEDWINDOW);

            ::SetWindowPos(m_Handle, HWND_NOTOPMOST,
                m_WindowRect.left,
                m_WindowRect.top,
                m_WindowRect.right - m_WindowRect.left,
                m_WindowRect.bottom - m_WindowRect.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE);

            ::ShowWindow(m_Handle, SW_NORMAL);
        }
    }
    void WindowContainer::ResizeWindow()
    {
        RECT clientRect;
        ::GetClientRect(m_Handle, &clientRect);
       
        int width = max(1u, clientRect.right - clientRect.left);
        int height = max(1u, clientRect.bottom - clientRect.top);
        if (width == m_Width && height == m_Height) return;

        m_Width = width;
        m_Height = height;
        WindowContainer::GetInstance().m_OnWindowResize(m_Width, m_Height);
    }
}


