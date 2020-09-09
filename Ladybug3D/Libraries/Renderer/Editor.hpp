#pragma once

struct ID3D12Device;
struct ID3D12GraphicsCommandList;

namespace Ladybug3D::D3D12 {
    class DescriptorHeapAllocator;
}

namespace Ladybug3D::Editor {

    void InitImGui(void* hwnd, ID3D12Device* device, unsigned int frameCount, Ladybug3D::D3D12::DescriptorHeapAllocator* descriptor);
    void ImGuiBegin();
    void ImGuiEnd(ID3D12GraphicsCommandList* cmdList);
    void DrawSceneGraph();
    void ShutDownImGui();
}