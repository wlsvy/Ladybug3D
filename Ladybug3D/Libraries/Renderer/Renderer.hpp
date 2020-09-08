#pragma once
#include "D3D12Resources.hpp"

namespace Ladybug3D {

    class Model;
    class Scene;
    class SceneObject;
    class Camera;

    struct CB_Matrix;
    struct CB_Test;

    class Renderer : public D3D12Resources
    {
    public:
        static Renderer* s_Ptr;

        Renderer();
        ~Renderer();

        bool OnInit(HWND hwnd, UINT width, UINT height);
        void OnUpdate();
        void OnRender();
        void OnDestroy();

        auto GetCurrentScene() const { return m_CurrentScene; }

    private:
        void CreateResourceView();
        void InitImGui(HWND hwnd);
        void CreateRootSignature();

        void RenderBegin();
        void RenderEnd();
        void Pass_Main();
        void Pass_ImGui();

        void ShutDownImGui();

        std::unique_ptr<Ladybug3D::D3D12::GraphicsCommandList> m_GraphicsCommandList;
        std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_ResourceDescriptorHeap;
        std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_ImGuiDescriptorHeap;
        std::unique_ptr<Ladybug3D::D3D12::VertexBuffer> m_VertexBuffer;
        std::unique_ptr<Ladybug3D::D3D12::IndexBuffer> m_IndexBuffer;

        std::vector<Model> m_Models;
        std::shared_ptr<Scene> m_CurrentScene;
        std::shared_ptr<SceneObject> m_Test;
        std::shared_ptr<Camera> m_MainCam;

        std::unique_ptr<Ladybug3D::D3D12::Texture> m_SampleTexture;
        std::unique_ptr<Ladybug3D::D3D12::ConstantBuffer<CB_Matrix>> m_CbMatrix;
        std::unique_ptr<Ladybug3D::D3D12::ConstantBuffer<CB_Test>> m_CbTest;

        

        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;



        void LoadAssets();
        void WaitForPreviousFrame();
    };

}

