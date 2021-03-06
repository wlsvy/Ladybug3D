#pragma once
#include <D3D12/D3D12_Define.hpp>
#include <D3D12/d3dx12.h>
#include <memory>

namespace Ladybug3D {
    class DX12Resources
    {
    public:
        static const UINT SWAPCHAIN_BUFFER_COUNT = 2;

        DX12Resources();
        virtual ~DX12Resources();

        void ResizeSwapChainBuffer(UINT width, UINT height);

        UINT GetWidth() const { return m_width; }
        UINT GetHeight() const { return m_height; }
        auto GetDevice() { return m_Device.Get(); }
        auto GetCommandQueue() {return m_CommandQueue.Get(); }

    protected:
        bool Initialize(HWND hwnd, UINT width, UINT height);
        void CreateDevice(IDXGIAdapter4* adapter);
        void CreateCommandQueue();
        void CreateSwapChain(HWND hwnd);
        void CreateMainRTV();
        void GetDebugInterface();
        void GetAdapters(bool useWarp);
        void GetHardwareAdapter(
            _In_ IDXGIFactory1* pFactory,
            _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
            bool requestHighPerformanceAdapter = false);

        void ClearMainRTV();

        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
        Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[SWAPCHAIN_BUFFER_COUNT];
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;

        std::unique_ptr<D3D12::GraphicsCommandList> m_GraphicsCommandList;
        std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_MainRTVDescriptorHeap;
        std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_DSVDescriptorHeap;
        std::unique_ptr<D3D12::Texture> m_DepthStencilTextures[SWAPCHAIN_BUFFER_COUNT];

        UINT m_FrameIndex;
        float m_ClearColor[4] = { 0.45f, 0.55f, 0.60f, 0.00f };

        UINT m_width;
        UINT m_height;
        float m_aspectRatio;
        CD3DX12_VIEWPORT m_viewport;
        CD3DX12_RECT m_scissorRect;

        bool m_useWarpDevice;
    };

}
