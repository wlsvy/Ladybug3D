//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "DXSample.h"
#include <D3D12/D3D12_Define.hpp>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace Ladybug3D {

    class Model;
    class Scene;
    class SceneObject;
    class Camera;

    class D3D12HelloTriangle : public DXSample
    {
    public:
        static const UINT SWAPCHAIN_BUFFER_COUNT = 2;
        static D3D12HelloTriangle* s_Ptr;

        D3D12HelloTriangle(UINT width, UINT height, std::wstring name);
        ~D3D12HelloTriangle();

        void OnInit(HWND hwnd, UINT width, UINT height);
        void OnUpdate();
        void OnRender();
        void OnDestroy();

        void Render();

        auto GetCurrentScene() const { return m_CurrentScene; }

    private:
        void CreateDevice(IDXGIAdapter4* adapter);
        void CreateCommandQueue();
        void CreateSwapChain(HWND hwnd);
        void CreateMainRTV();
        void CreateResourceView();
        void GetDebugInterface();
        void GetAdapters(bool useWarp);
        void InitImGui(HWND hwnd);
        void CreateRootSignature();
        void ClearMainRTV();


        struct Vertex
        {
            XMFLOAT3 position;
            XMFLOAT4 color;
        };

        std::unique_ptr<Ladybug3D::D3D12::GraphicsCommandList> m_GraphicsCommandList;
        std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_ResourceDescriptorHeap;
        std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_MainRTVDescriptorHeap;
        std::unique_ptr<Ladybug3D::D3D12::VertexBuffer> m_VertexBuffer;
        std::unique_ptr<Ladybug3D::D3D12::IndexBuffer> m_IndexBuffer;

        std::vector<Model> m_Models;
        std::shared_ptr<Scene> m_CurrentScene;
        std::shared_ptr<SceneObject> m_Test;
        std::shared_ptr<Camera> m_MainCam;

        // Pipeline objects.
        CD3DX12_VIEWPORT m_viewport;
        CD3DX12_RECT m_scissorRect;
        ComPtr<IDXGISwapChain3> m_swapChain;
        ComPtr<ID3D12Device> m_Device;
        ComPtr<ID3D12Resource> m_renderTargets[SWAPCHAIN_BUFFER_COUNT];
        ComPtr<ID3D12CommandAllocator> m_commandAllocator;
        ComPtr<ID3D12CommandQueue> m_CommandQueue;
        ComPtr<ID3D12RootSignature> m_rootSignature;
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        ComPtr<ID3D12PipelineState> m_pipelineState;
        Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter;
        UINT m_rtvDescriptorSize;

        // App resources.
        ComPtr<ID3D12Resource> m_vertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

        // Synchronization objects.
        UINT m_frameIndex;
        HANDLE m_fenceEvent;
        ComPtr<ID3D12Fence> m_fence;
        UINT64 m_fenceValue;


        UINT m_FrameIndex;
        float m_ClearColor[4] = { 0.45f, 0.55f, 0.60f, 0.00f };

        void LoadPipeline(HWND hwnd);
        void LoadAssets();
        void PopulateCommandList();
        void WaitForPreviousFrame();
    };

}

