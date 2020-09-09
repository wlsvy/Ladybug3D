#pragma once
#include <D3D12/D3D12_Define.hpp>
#include <memory>

namespace Ladybug3D {

    class Editor {
    public:
        static constexpr UINT EDITOR_DESCRIPTOR_SIZE = 64;

        void Initialize(void* hwnd, ID3D12Device* device, unsigned int frameCount);
        void NewFrame();
        void Render(ID3D12GraphicsCommandList* cmdList);
        void DrawSampleWindow();
        void DrawSceneGraph();
        void ShutDownImGui();

        ID3D12DescriptorHeap* GetDescriptorHeap();

    private:
        std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_DescriptorHeap;

    };
}