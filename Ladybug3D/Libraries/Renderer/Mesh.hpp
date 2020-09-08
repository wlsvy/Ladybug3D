#pragma once
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <D3D12/D3D12_Define.hpp>
#include <DirectXMath.h>
#include <D3D12/D3D12_Texture.hpp>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiAnimation;
struct BoneChannel;
class AnimationClip;
class SkinnedModel;



namespace Ladybug3D {

	class Mesh {
	public:
		Mesh(std::shared_ptr<D3D12::VertexBuffer>& vertexBuffer, std::shared_ptr<D3D12::IndexBuffer>& indexBuffer, const DirectX::XMMATRIX& worldMatrix)
			: m_VertexBuffer(vertexBuffer)
			, m_IndexBuffer(indexBuffer) 
			, m_WorldMatrix(worldMatrix)
		{}

		auto GetVertexBuffer() const { return m_VertexBuffer.get(); }
		auto GetVertexBufferView() const { return m_VertexBuffer.get()->GetView(); }
		auto GetIndexBuffer() const { return m_IndexBuffer.get(); }
		auto GetIndexBufferView() const  { return m_IndexBuffer.get()->GetView(); }
		auto GetWorldMatrix() const { return m_WorldMatrix; }
			
	private:
		std::shared_ptr<D3D12::VertexBuffer> m_VertexBuffer;
		std::shared_ptr<D3D12::IndexBuffer> m_IndexBuffer;
		DirectX::XMMATRIX m_WorldMatrix;
	};

	class Model {
	public:
		Model() {}
		Model(const Mesh& mesh) : m_Meshes({ mesh }) {}
		Model(std::vector<Mesh> && meshes) : m_Meshes(std::move(meshes)) {}

		auto& GetMeshes() const { return m_Meshes; }

	private:
		std::vector<Mesh> m_Meshes;
	};

	class ManagerBase {
	public:
		ManagerBase();
		~ManagerBase();
		void CreateRootSignature(ID3D12Device* device);

	protected:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		std::unique_ptr<Ladybug3D::D3D12::DescriptorHeapAllocator> m_DescriptorHeap;
	};

	class TextureManager : public ManagerBase {
	public:

		void AddTexture(const wchar_t* filePath);
		void Erase(UINT offset);
		void Clear();
		//auto GetShaderResourceView(UINT offset) { return m_DescriptorHeap->GetGpuHandle(offset); }
		void SetDescriptorHandle(D3D12::GraphicsCommandList* cmdList, UINT offset);

	private:
		std::map<int, std::shared_ptr<Ladybug3D::D3D12::Texture>> m_TextureMap;
	};

	class CBVManager {

	};

	class ShaderManager {

	};



	class ModelImporter {
	public:
		ModelImporter(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) : m_Device(device), m_CommandList(cmdList) {}
		Model CreateModel(const std::string& filePath, const aiScene* scene);
			
	private:
		void ProcessNode(aiNode* node, const aiScene* scene, const DirectX::XMMATRIX& parentTransformMatrix);

		ID3D12Device* m_Device;
		ID3D12GraphicsCommandList* m_CommandList;
		std::vector<Mesh> m_Meshes;
		std::shared_ptr<Mesh> m_Mesh;
	};

	Model LoadModel(const std::string& filePath, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
}