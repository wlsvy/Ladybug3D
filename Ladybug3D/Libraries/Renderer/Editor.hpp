#pragma once

struct ID3D12Device;

namespace Ladybug3D::D3D12 {
    class DescriptorHeapAllocator;
}

namespace Ladybug3D::Editor {

    void InitImGui(void* hwnd, ID3D12Device* device, unsigned int frameCount, Ladybug3D::D3D12::DescriptorHeapAllocator* descriptor);
    void DrawSceneGraph();
    void ShutDownImGui();
}