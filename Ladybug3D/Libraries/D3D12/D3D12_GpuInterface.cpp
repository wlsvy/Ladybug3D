#include "D3D12_GpuInterface.hpp"

#include <iostream>
#include <array>
#include <d3d12.h>
#include "d3dx12.h"
#include "D3D12_Util.hpp"

using namespace std;
using namespace Microsoft::WRL;

namespace Ladybug3D::D3D12 {

	GpuInterface::GpuInterface()
	{
	}

	GpuInterface::~GpuInterface() {
        ::CloseHandle(m_fenceEvent);
	}

	bool GpuInterface::Initialize(HWND hwnd, UINT width, UINT height)
	{
        try {
            m_Width = width;
            m_Height = height;
            m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

            LoadPipeline(hwnd);

            ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
            ThrowIfFailed(m_commandList->Close());
            ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

            m_fenceValue = 1;
            m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (m_fenceEvent == nullptr)
            {
                ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
            }
        }
        catch (HrException& e) {
            cout << e.what() << endl;
            return false;
        }
        return true;
	}

	void GpuInterface::OnUpdate()
	{
	}

	void GpuInterface::OnRender()
	{
        auto backBuffer = m_renderTargets[m_frameIndex].Get();
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);

        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

        ThrowIfFailed(m_commandAllocator->Reset());
        ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer,
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_commandList->ResourceBarrier(1, &barrier);

        
        m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
        const float clearColor[] = { 0.45f, 0.55f, 0.60f, 1.00f };
        m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        m_commandList->SetDescriptorHeaps(1, m_srvHeap.GetAddressOf());

        barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer,
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_commandList->ResourceBarrier(1, &barrier);
        ThrowIfFailed(m_commandList->Close());

        //PopulateCommandList();
        ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // Present the frame.
        ThrowIfFailed(m_swapChain->Present(1, 0));

        WaitForPreviousFrame();
	}

	void GpuInterface::OnDestroy()
	{
	}

    void GpuInterface::GetDebugInterface()
    {
        ComPtr<ID3D12Debug> debugInterface;
        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
        debugInterface->EnableDebugLayer();
    }

    void GpuInterface::GetAdapters(bool useWarp)
    {
        ComPtr<IDXGIFactory4> factory;
        UINT createFactoryFlags = 0;
#if defined(_DEBUG)
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

        ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory)));

        ComPtr<IDXGIAdapter1> adapter1;
        if (useWarp)
        {
            ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter1)));
            ThrowIfFailed(adapter1.As(&m_Adapter));
        }
        else
        {
            GetHardwareAdapter(factory.Get(), adapter1.GetAddressOf());
            ThrowIfFailed(adapter1.As(&m_Adapter));
        }
    }

    void GpuInterface::CreateDevice(IDXGIAdapter4* adapter)
    {
        ComPtr<ID3D12Device2> d3d12Device2;
        ThrowIfFailed(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_device.GetAddressOf())));

        // Enable debug messages in debug mode.
#if defined(_DEBUG)
        ComPtr<ID3D12InfoQueue> pInfoQueue;
        if (SUCCEEDED(m_device.As(&pInfoQueue)))
        {
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

            D3D12_MESSAGE_SEVERITY Severities[] =
            {
                D3D12_MESSAGE_SEVERITY_INFO
            };

            D3D12_MESSAGE_ID DenyIds[] = {
                D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   
                D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         
                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       
            };

            D3D12_INFO_QUEUE_FILTER NewFilter = {};
            NewFilter.DenyList.NumSeverities = _countof(Severities);
            NewFilter.DenyList.pSeverityList = Severities;
            NewFilter.DenyList.NumIDs = _countof(DenyIds);
            NewFilter.DenyList.pIDList = DenyIds;

            ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
        }
#endif
    }

    void GpuInterface::CreateCommandQueue()
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
    }

    void GpuInterface::CreateSwapChain(HWND hwnd)
    {
        UINT createFactoryFlags = 0;
#if defined(_DEBUG)
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

        ComPtr<IDXGIFactory4> factory4;
        ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory4)));

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = FrameCount;
        swapChainDesc.Width = m_Width;
        swapChainDesc.Height = m_Height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain1;
        ThrowIfFailed(factory4->CreateSwapChainForHwnd(
            m_commandQueue.Get(),
            hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1));

        // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen will be handled manually.
        ThrowIfFailed(factory4->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
        ThrowIfFailed(swapChain1.As(&m_swapChain));
    }

    void GpuInterface::LoadPipeline(HWND hwnd)
	{
        UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
        GetDebugInterface();
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

        GetAdapters(false);
        CreateDevice(m_Adapter.Get());
        CreateCommandQueue();
        CreateSwapChain(hwnd);


        // Create descriptor heaps.
        {
            // Describe and create a render target view (RTV) descriptor heap.
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.NumDescriptors = FrameCount;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtvHeap)));

            m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.NumDescriptors = 1;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            ThrowIfFailed(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_srvHeap)));
        }
        

        // Create frame resources.
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

            // Create a RTV for each frame.
            for (UINT n = 0; n < FrameCount; n++)
            {
                ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
                m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
                rtvHandle.Offset(1, m_rtvDescriptorSize);
            }
        }

        ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
	}
	void GpuInterface::LoadAssets()
	{
	}
	void GpuInterface::PopulateCommandList()
	{
        ThrowIfFailed(m_commandAllocator->Reset());
        ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
        const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

        ThrowIfFailed(m_commandList->Close());
	}

	void GpuInterface::WaitForPreviousFrame()
	{
        const UINT64 fence = m_fenceValue;
        ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
        m_fenceValue++;

        // Wait until the previous frame is finished.
        if (m_fence->GetCompletedValue() < fence)
        {
            ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
            WaitForSingleObject(m_fenceEvent, INFINITE);
        }
	}

    void GpuInterface::RenderBegin()
    {
        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

        ThrowIfFailed(m_commandAllocator->Reset());
        //ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));
        ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_renderTargets[m_frameIndex].Get();
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

        /*auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_renderTargets[m_frameIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);*/
        m_commandList->ResourceBarrier(1, &barrier);
    }

    void GpuInterface::RenderEnd()
    {
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_renderTargets[m_frameIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_commandList->ResourceBarrier(1, &barrier);
        ThrowIfFailed(m_commandList->Close());

        //PopulateCommandList();
        ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

    void GpuInterface::SetRenderTarget(UINT numDesriptors, ID3D12DescriptorHeap* rtv, bool isSingleToRange, const D3D12_CPU_DESCRIPTOR_HANDLE* dsv)
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtv->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
        m_commandList->OMSetRenderTargets(numDesriptors, &rtvHandle, isSingleToRange , dsv);
    }

    void GpuInterface::ClearRenderTarget(ID3D12DescriptorHeap* rtv, float* clearColor)
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtv->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
        m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    }

    void GpuInterface::PresentSwapChain()
    {
        ThrowIfFailed(m_swapChain->Present(1, 0));
        WaitForPreviousFrame();
    }

    void GpuInterface::GetHardwareAdapter(
        _In_ IDXGIFactory1* pFactory,
        _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
        bool requestHighPerformanceAdapter)
    {
        *ppAdapter = nullptr;

        ComPtr<IDXGIAdapter1> adapter;

        ComPtr<IDXGIFactory6> factory6;
        if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            for (
                UINT adapterIndex = 0;
                DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
                    adapterIndex,
                    requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                    IID_PPV_ARGS(&adapter));
                ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    continue;
                }

                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }
        else
        {
            for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    continue;
                }
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }

        *ppAdapter = adapter.Detach();
    }
}