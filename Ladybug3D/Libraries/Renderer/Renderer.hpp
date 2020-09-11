#pragma once
#include "DX12Resources.hpp"
#include "Singleton.hpp"

namespace Ladybug3D {

    class Model;
    class Scene;
    class SceneObject;
    class Camera;
    class Editor;

    struct CB_PerObject;
    struct CB_PerScene;

    class Renderer : public DX12Resources, public Singleton<Renderer>
    {
    public:
        Renderer();
        ~Renderer();

        bool OnInit(HWND hwnd, UINT width, UINT height);
        void OnUpdate();
        void OnRender();
        void OnDestroy();

        auto GetCurrentScene() const { return m_CurrentScene; }
        auto GetGraphicsCommandList() { return m_GraphicsCommandList.get(); }

    private:
        void CreateRootSignature();
        void LoadAssets();
        void CreatePipelineState();
        void CreateResourceView();

        void RenderBegin();
        void RenderEnd();
        void Pass_Main();
        void Pass_Editor();
        void WaitForPreviousFrame();


        std::unique_ptr<Ladybug3D::D3D12::GraphicsCommandList> m_GraphicsCommandList;
        std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_ResourceDescriptorHeap;

        std::unique_ptr<Ladybug3D::D3D12::ConstantBuffer<CB_PerScene>> m_CB_PerScene;
        std::unique_ptr<Ladybug3D::D3D12::ConstantBuffer<CB_PerObject>> m_CB_PerObject;

        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

        std::shared_ptr<Scene> m_CurrentScene;
        std::shared_ptr<Camera> m_MainCam;

        std::unique_ptr<Ladybug3D::Editor> m_Editor;
    };
}

