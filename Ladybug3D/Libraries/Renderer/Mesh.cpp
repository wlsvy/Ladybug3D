#include "Mesh.hpp"
#include <D3D12/D3D12_DescriptorHeapAllocator.hpp>
#include <D3D12/D3D12_Util.hpp>
#include <D3D12/D3D12_Texture.hpp>

#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <Assimp/scene.h>

using namespace Microsoft::WRL;
using namespace Ladybug3D::D3D12;
using namespace std;

namespace Ladybug3D::Renderer {
	ManagerBase::ManagerBase()
	{
	}
	ManagerBase::~ManagerBase()
	{
	}

	void ImGuiRootSig() {

	}

	void ManagerBase::CreateRootSignature(ID3D12Device* device)
	{
		CD3DX12_DESCRIPTOR_RANGE range[] = 
		{
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0),	//MVP matrix
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0),	//Texture
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0)		//CBV for Test
		};

		CD3DX12_ROOT_PARAMETER rootParameters[3];
		rootParameters[0].InitAsDescriptorTable(1, &range[0], D3D12_SHADER_VISIBILITY_VERTEX);
		rootParameters[1].InitAsDescriptorTable(1, &range[1], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[2].InitAsDescriptorTable(1, &range[2], D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc[2];
		samplerDesc[0].Init(0, D3D12_FILTER_ANISOTROPIC);
		samplerDesc[1].Init(1, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR);

		auto rootSignatureFlag =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | 
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | 
			D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.NumParameters = _countof(rootParameters);
		rootSignatureDesc.pParameters = rootParameters;
		rootSignatureDesc.NumStaticSamplers = _countof(samplerDesc);
		rootSignatureDesc.pStaticSamplers = samplerDesc;
		rootSignatureDesc.Flags = rootSignatureFlag;

		ComPtr<ID3DBlob> rootSigBlob;
		ComPtr<ID3DBlob> errorBlob;

		ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSigBlob, &errorBlob));
		ThrowIfFailed(device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
	}

	void ProcessMesh(aiMesh* mesh, vector<Vertex>& vertices, vector<UINT>& indices)
	{
		vertices.clear();
		indices.clear();
		vertices.reserve(mesh->mNumVertices);
		indices.reserve(mesh->mNumFaces * 3);

		//Get vertices
		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

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

			vertices.push_back(vertex);
		}

		//Get indices
		for (UINT i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (UINT j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
	}
	void ProcessNode(aiNode* node, const aiScene* scene, const DirectX::XMMATRIX& parentTransformMatrix)
	{
		DirectX::XMMATRIX nodeTransformMatrix = DirectX::XMMatrixTranspose(DirectX::XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;

		for (UINT i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			vector<Vertex> vertices;
			vector<UINT> indices;
			ProcessMesh(mesh, vertices, indices);

			
		}

		for (UINT i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene, nodeTransformMatrix);
		}
	}

	Model LoadModel(const std::string& filePath, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
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
			return Model();
		}

		auto modelImporter = ModelImporter(device, cmdList);
		return modelImporter.CreateModel(filePath, pScene);
		//return true;
	}
	Model LoadModel2(const std::string& filePath, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
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
			return Model();
		}

		auto modelImporter = ModelImporter(device, cmdList);
		return modelImporter.CreateModel(filePath, pScene);
	}
	void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, const DirectX::XMMATRIX& parentTransformMatrix)
	{
		DirectX::XMMATRIX nodeTransformMatrix = DirectX::XMMatrixTranspose(DirectX::XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;
		
		for (UINT i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
			vector<Vertex> vertices;
			vector<UINT> indices;
			ProcessMesh(aimesh, vertices, indices);
			auto mesh = MeshReal(
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
	Model ModelImporter::CreateModel(const std::string& filePath, const aiScene* scene)
	{
		this->ProcessNode(scene->mRootNode, scene, DirectX::XMMatrixIdentity());
		return Model(move(m_Meshes));
	}
}