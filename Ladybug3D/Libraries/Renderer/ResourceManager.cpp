#include "ResourceManager.hpp"
#include "Model.hpp"
#include "Renderer.hpp"

#include <filesystem>

#include <D3D12/D3D12_DescriptorHeapAllocator.hpp>
#include <D3D12/D3D12_CommandList.hpp>
#include <D3D12/D3D12_Util.hpp>
#include <D3D12/D3D12_Texture.hpp>
#include <D3D12/D3D12_VertexType.hpp>

#include <Direct12XTK/Include/ResourceUploadBatch.h>

#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <Assimp/scene.h>

using namespace Microsoft::WRL;
using namespace Ladybug3D::D3D12;
using namespace std;

namespace Ladybug3D {
	ResourceManager::ResourceManager()
	{
	}
	ResourceManager::~ResourceManager()
	{
	}

	bool ResourceManager::Initialize()
	{
		LoadModels();
		LoadTextures();
		return true;
	}

	void ResourceManager::LoadTextures()
	{
		auto device = Renderer::GetInstance().GetDevice();
		auto commandQueue = Renderer::GetInstance().GetCommandQueue();

		DirectX::ResourceUploadBatch uploadBatch(device);
		uploadBatch.Begin();

		for (auto& resource : filesystem::recursive_directory_iterator(LADYBUG3D_RESOURCE_PATH)) {
			if (auto extension = resource.path().extension(); 
				extension != L".png" &&
				extension != L".jpg") 
			{
				continue;
			}

			cout << "Find Texture At " << resource << endl;
			auto stem = resource.path().stem().string();
				
			if (m_TextureMap.find(stem) != m_TextureMap.end()) {
				cout << "Texture : " << stem << " or same named Texture is already loaded" << endl;
				continue;
			}

			auto texture = make_shared<Texture>();
			texture->InitializeWICTexture(resource.path().c_str(), uploadBatch, device);
			m_TextureMap[stem] = texture;
		}

		auto finish = uploadBatch.End(commandQueue);
		finish.wait();
	}

	void ResourceManager::LoadModels()
	{
		auto device = Renderer::GetInstance().GetDevice();
		auto cmdList = Renderer::GetInstance().GetGraphicsCommandList();

		for (auto& resource : filesystem::recursive_directory_iterator(LADYBUG3D_RESOURCE_PATH)) {
			if (auto extension = resource.path().extension(); 
				extension != L".obj")
			{
				continue;
			}

			cout << "Find Obj Model At " << resource.path().string() << endl;
			auto stem = resource.path().stem().string();

			if (m_ModelMap.find(stem) != m_ModelMap.end()) {
				cout << "Model : " << stem << " or same named Model is already loaded" << endl;
				continue;
			}

			m_ModelMap[stem] = LoadModel(resource.path().string(), device, cmdList);
		}
	}

	void ProcessMesh(aiMesh* mesh, vector<Vertex3D>& vertices, vector<UINT>& indices)
	{
		vertices.clear();
		vertices.reserve(static_cast<size_t>(mesh->mNumVertices));

		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			vertices.emplace_back();
			auto& vertex = vertices.back();

			vertex.pos.x = mesh->mVertices[i].x;
			vertex.pos.y = mesh->mVertices[i].y;
			vertex.pos.z = mesh->mVertices[i].z;

			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;

			if (mesh->mTangents) {
				vertex.tangent.x = mesh->mTangents[i].x;
				vertex.tangent.y = mesh->mTangents[i].y;
				vertex.tangent.z = mesh->mTangents[i].z;
			}

			if (mesh->mTextureCoords[0])
			{
				vertex.uv.x = (float)mesh->mTextureCoords[0][i].x;
				vertex.uv.y = (float)mesh->mTextureCoords[0][i].y;
			}
		}

		indices.clear();
		indices.reserve(static_cast<size_t>(mesh->mNumFaces) * 3);

		for (UINT i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (UINT j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
	}

	class ModelImporter {
	public:
		ModelImporter(ID3D12Device* device, GraphicsCommandList* cmdList) : m_Device(device), m_CommandList(cmdList) {}
		shared_ptr<Model> CreateModel(const std::string& filePath, const aiScene* scene);

	private:
		void ProcessNode(aiNode* node, const aiScene* scene, const DirectX::XMMATRIX& parentTransformMatrix);

		ID3D12Device* m_Device;
		GraphicsCommandList* m_CommandList;
		std::vector<Mesh> m_Meshes;
		std::shared_ptr<Mesh> m_Mesh;
	};

	
	void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, const DirectX::XMMATRIX& parentTransformMatrix)
	{
		DirectX::XMMATRIX nodeTransformMatrix = DirectX::XMMatrixTranspose(DirectX::XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;

		for (UINT i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
			vector<Vertex3D> vertices;
			vector<UINT> indices;
			ProcessMesh(aimesh, vertices, indices);
			auto mesh = Mesh(
				make_shared<VertexBuffer>(m_Device, m_CommandList, vertices),
				make_shared<IndexBuffer>(m_Device, m_CommandList, indices),
				nodeTransformMatrix);
			m_Meshes.push_back(mesh);
		}

		for (UINT i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene, nodeTransformMatrix);
		}
	}
	shared_ptr<Model> ModelImporter::CreateModel(const std::string& filePath, const aiScene* scene)
	{
		this->ProcessNode(scene->mRootNode, scene, DirectX::XMMatrixIdentity());
		return make_shared<Model>(move(m_Meshes));
	}

	shared_ptr<Model> LoadModel(const std::string& filePath, ID3D12Device* device, GraphicsCommandList* cmdList)
	{
		const auto importer_flags =
			aiProcess_MakeLeftHanded |              // directx style.
			aiProcess_FlipUVs |                     // directx style.
			aiProcess_FlipWindingOrder |            // directx style.
			aiProcess_CalcTangentSpace |
			aiProcess_GenSmoothNormals |
			aiProcess_JoinIdenticalVertices |
			aiProcess_OptimizeMeshes |              // reduce the number of meshes         
			aiProcess_ImproveCacheLocality |        // re-order triangles for better vertex cache locality.
			aiProcess_RemoveRedundantMaterials |    // remove redundant/unreferenced materials.
			aiProcess_LimitBoneWeights |
			aiProcess_SplitLargeMeshes |
			aiProcess_Triangulate |
			aiProcess_GenUVCoords |
			aiProcess_SortByPType |                 // splits meshes with more than one primitive type in homogeneous sub-meshes.
			aiProcess_FindDegenerates |             // convert degenerate primitives to proper lines or points.
			aiProcess_FindInvalidData |
			aiProcess_FindInstances |
			aiProcess_ValidateDataStructure;

		Assimp::Importer importer;
		const aiScene* pScene = importer.ReadFile(
			filePath,
			aiProcessPreset_TargetRealtime_Fast |
			aiProcess_ConvertToLeftHanded);

		if (pScene == nullptr ||
			!pScene->HasMeshes())
		{
			return shared_ptr<Model>();
		}

		auto modelImporter = ModelImporter(device, cmdList);
		return modelImporter.CreateModel(filePath, pScene);
	}
}