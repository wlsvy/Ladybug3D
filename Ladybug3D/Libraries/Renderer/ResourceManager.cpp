#include "ResourceManager.hpp"
#include "Model.hpp"
#include "Renderer.hpp"

#include <D3D12/D3D12_DescriptorHeapAllocator.hpp>
#include <D3D12/D3D12_CommandList.hpp>
#include <D3D12/D3D12_Util.hpp>
#include <D3D12/D3D12_Texture.hpp>
#include <D3D12/D3D12_VertexType.hpp>
#include <D3D12/DDSTextureLoader.h>

#include <Direct12XTK/Include/ResourceUploadBatch.h>

#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <Assimp/scene.h>

using namespace Microsoft::WRL;
using namespace Ladybug3D::D3D12;
using namespace std;

namespace Ladybug3D {

	shared_ptr<Model> LoadModel(const std::string& filePath, ID3D12Device* device, GraphicsCommandList* cmdList);


	ResourceManager::ResourceManager()
	{
	}
	ResourceManager::~ResourceManager()
	{
	}

	bool ResourceManager::Initialize()
	{
		TrackAssetsPath();
		LoadModels();
		LoadTextures();
		return true;
	}

	void ResourceManager::Destroy()
	{
		m_ModelMap.clear();
		m_TextureMap.clear();
	}

	shared_ptr<Model> ResourceManager::GetModel(const string& name)
	{
		if (auto iter = m_ModelMap.find(name); iter != m_ModelMap.end()) {
			return iter->second;
		}
		return shared_ptr<Model>();
	}

	shared_ptr<Texture> ResourceManager::GetTexture(const string& name)
	{
		if (auto iter = m_TextureMap.find(name); iter != m_TextureMap.end()) {
			return iter->second;
		}
		return shared_ptr<Texture>();
	}

	const wchar_t* ResourceManager::GetShaderPath(const string& name)
	{
		if (auto iter = m_ShaderPathMap.find(name); iter != m_ShaderPathMap.end()) {
			return iter->second.c_str();
		}
		return nullptr;
	}

	const wchar_t* ResourceManager::GetTexturePath(const std::string& name)
	{
		if (auto iter = m_TexturePathMap.find(name); iter != m_TexturePathMap.end()) {
			return iter->second.c_str();
		}
		return nullptr;
	}

	void ResourceManager::TrackAssetsPath()
	{
		for (auto& resource : filesystem::recursive_directory_iterator(LADYBUG3D_RESOURCE_PATH)) {
			auto extension = resource.path().extension();
			auto stem = resource.path().stem().string();

			if (extension == L".png" ||
				extension == L".jpg")
			{
				cout << "Find Texture At " << resource << endl;
				if (m_TexturePathMap.find(stem) != m_TexturePathMap.end()) {
					cout << "Texture : " << stem << " or same named Texture is already found" << endl;
					continue;
				}
				m_TexturePathMap[stem] = resource.path();
			}
			else if (extension == L".dds") {
				cout << "Find DDS Texture At " << resource << endl;
				if (m_DDSTexturePathMap.find(stem) != m_DDSTexturePathMap.end()) {
					cout << "Texture : " << stem << " or same named Texture is already found" << endl;
					continue;
				}
				m_DDSTexturePathMap[stem] = resource.path();
			}
			else if (extension == L".obj" ||
				extension == L".fbx")
			{
				cout << "Find Model At " << resource << endl;
				if (m_ModelPathMap.find(stem) != m_ModelPathMap.end()) {
					cout << "Model : " << stem << " or same named Model is already found" << endl;
					continue;
				}
				m_ModelPathMap[stem] = resource.path();
			}
			else if (extension == L".hlsl")
			{
				cout << "Find Shader At " << resource << endl;
				if (m_ShaderPathMap.find(stem) != m_ShaderPathMap.end()) {
					cout << "Shader : " << stem << " or same named Shader is already found" << endl;
					continue;
				}
				m_ShaderPathMap[stem] = resource.path();
			}
		}
	}

	void ResourceManager::LoadTextures()
	{
		auto device = Renderer::GetInstance().GetDevice();
		auto cmdList = Renderer::GetInstance().GetGraphicsCommandList();
		auto commandQueue = Renderer::GetInstance().GetCommandQueue();

		DirectX::ResourceUploadBatch uploadBatch(device);
		uploadBatch.Begin();

		for (auto& pair : m_TexturePathMap) {
			cout << "Load Texture : " << pair.second << endl;
			auto texture = make_shared<Texture>();
			texture->InitializeWICTexture(pair.second.c_str(), uploadBatch, device);
			m_TextureMap[pair.first] = texture;
		}

		auto finish = uploadBatch.End(commandQueue);

		for (auto& pair : m_DDSTexturePathMap) {
			cout << "Load Texture : " << pair.second << endl;

			auto texture = make_shared<Texture>();
			ComPtr<ID3D12Resource> uploadHeap;

			DirectX::CreateDDSTextureFromFile12(device, cmdList->GetCommandList(), pair.second.c_str(), texture->GetResourceAddress(), uploadHeap.GetAddressOf());
			cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texture->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_GENERIC_READ));
			cmdList->TrackObject(uploadHeap);

			m_TextureMap[pair.first] = texture;
		}

		finish.wait();
	}

	void ResourceManager::LoadModels()
	{
		auto device = Renderer::GetInstance().GetDevice();
		auto cmdList = Renderer::GetInstance().GetGraphicsCommandList();

		for (auto& pair : m_ModelPathMap) {
			cout << "Load Model : " << pair.second << endl;
			m_ModelMap[pair.first] = LoadModel(pair.second.string(), device, cmdList);
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