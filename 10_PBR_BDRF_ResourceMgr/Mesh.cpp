
#include "Mesh.h"
#include "..\Common\Helper.h"

#include <assimp\mesh.h>
#include <map>
#include <assimp\postprocess.h>
#include <assimp\Importer.hpp>
#include <filesystem>

using namespace DirectX;

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pBWVertexBuffer);
}

void Mesh::Create(aiMesh* mesh, SkeletonResource* skeleton)
{
	m_MaterialIndex = mesh->mMaterialIndex;
	m_Name = mesh->mName.C_Str();

	// 버텍스 정보 생성
	m_Vertices.resize(mesh->mNumVertices);
	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		m_Vertices[i].Position = Vector4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
		m_Vertices[i].TexCoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		m_Vertices[i].Normal   = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		m_Vertices[i].Tangent  = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
	}
	CreateVertexBuffer(&m_Vertices[0], (UINT)m_Vertices.size());


	//=======================================================//
	// 인덱스 정보 생성
	unique_ptr<UINT[]> indices(new UINT[mesh->mNumFaces * 3]);

	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
		indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
	}
	CreateIndexBuffer(&m_Faces[0], (UINT)m_Faces.size());
}

void Mesh::CreateBoneWeightVertex(ID3D11Device* device, aiMesh* mesh)
{
	m_MaterialIndex = mesh->mMaterialIndex;

	// 버텍스 정보 생성	
	m_BoneWeightVertices.resize(mesh->mNumVertices);

	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		m_BoneWeightVertices[i].Position = Vector4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
		m_BoneWeightVertices[i].TexCoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		m_BoneWeightVertices[i].Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		m_BoneWeightVertices[i].Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
	}

	UINT meshBoneCount = mesh->mNumBones;     // 메쉬와 연결된 본 개수
	m_BoneReferences.resize(meshBoneCount);   // 본 연결 정보 컨테이너 크기 조절

	// 메쉬와 연결된 본들을 처리
	UINT boneIndexCounter = 0;
	std::map<std::string, int> BoneMapping;

	for (UINT i = 0; i < meshBoneCount; ++i)
	{
		aiBone* bone = mesh->mBones[i];
		std::string boneName = bone->mName.C_Str();

		UINT boneIndex = 0;

		// 매핑이 아직 안돼있다면,
		if (BoneMapping.find(boneName) == BoneMapping.end())
		{
			// Map bone name to bone Index
			boneIndex = boneIndexCounter;
			boneIndexCounter++;

			m_BoneReferences[boneIndex].NodeName = boneName;
			m_BoneReferences[boneIndex].OffsetMatrix = Matrix(&bone->mOffsetMatrix.a1).Transpose();
			BoneMapping[boneName] = boneIndex;
		}
		else
			boneIndex = BoneMapping[boneName];

		// 본과 연결된 버텍스들을 처리
		for (UINT j = 0; j < bone->mNumWeights; ++j)
		{
			UINT vertexID = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;
			m_BoneWeightVertices[vertexID].AddBoneData(boneIndex, weight);
		}
	}

	CreateBoneWeightVertexBuffer(device, &m_BoneWeightVertices[0], (UINT)m_BoneWeightVertices.size());

	//=======================================================//
	// 인덱스 정보 생성
	unique_ptr<UINT[]> indices(new UINT[mesh->mNumFaces * 3]);

	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
		indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
	}

	/// CreateIndexBuffer  ///
	// 인덱스 개수 저장.
	m_IndexCount = mesh->mNumFaces * 3;

	D3D11_BUFFER_DESC indexBD = {};
	indexBD.ByteWidth = sizeof(UINT) * mesh->mNumFaces * 3;
	indexBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBD.Usage = D3D11_USAGE_DEFAULT;
	indexBD.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices.get();
	HR_T(device->CreateBuffer(&indexBD, &ibData, &m_pIndexBuffer));
}

void Mesh::UpdateMatrixPalette(Matrix* MatrixPalettePtr)
{
	assert(m_BoneReferences.size() < 128);

	for (UINT i = 0; i < m_BoneReferences.size(); ++i)
	{
		Matrix& BoneNodeWorldMatrix = *m_BoneReferences[i].NodeWorldMatrixPtr;

		// HLSL 상수버퍼에 업데이트할 때, 바로 복사할 수 있도록 전치해서 저장.
		MatrixPalettePtr[i] = (m_BoneReferences[i].OffsetMatrix * BoneNodeWorldMatrix).Transpose();
	}
}

void Mesh::CreateBoneWeightVertexBuffer(ID3D11Device* device, BoneWeightVertex* boneWV, UINT size)
{
	// Create the vertex buffer for bone weights
	D3D11_BUFFER_DESC vertexBD = {};
	vertexBD.ByteWidth = sizeof(BoneWeightVertex) * size;
	vertexBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBD.Usage = D3D11_USAGE_DEFAULT;
	vertexBD.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = boneWV;
	HR_T(device->CreateBuffer(&vertexBD, &vbData, &m_pBWVertexBuffer));

	// 버텍스 버퍼 정보
	m_VertexCount = size;
	m_VertexBufferStride = sizeof(BoneWeightVertex);
	m_VertexBufferOffset = 0;
}

void Mesh::CreateIndexBuffer(Face* faces, UINT faceCount)
{
	/// CreateIndexBuffer  ///
	// 인덱스 개수 저장.
	m_IndexCount = faceCount;

	D3D11_BUFFER_DESC indexBD = {};
	indexBD.ByteWidth = sizeof(UINT) * faceCount;
	indexBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBD.Usage = D3D11_USAGE_DEFAULT;
	indexBD.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = faces;
	HR_T(D3DRenderManager::m_pDevice->CreateBuffer(&indexBD, &ibData, &m_pIndexBuffer));
}



bool SkeletalMeshSceneResource::Create(std::string filePath)
{
	std::filesystem::path path = ToWString(string(filePath));

	Assimp::Importer importer;

	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);	// $assimp_fbx$ 노드 생성안함

	unsigned int importFlags = aiProcess_Triangulate | // 삼각형으로 변환
		aiProcess_GenNormals |   // 노말 생성
		aiProcess_GenUVCoords |  // UV 생성
		aiProcess_CalcTangentSpace |  // 탄젠트 생성
		aiProcess_LimitBoneWeights |  // 본의 영향을 받는 정점의 최대 개수를 4개로 제한
		aiProcess_GenBoundingBoxes | // 바운딩 박스 생성
		aiProcess_ConvertToLeftHanded;   // 왼손 좌표계로 변환

	const aiScene* m_fbxModel = importer.ReadFile(filePath, importFlags);

	if (!m_fbxModel)
	{
		LOG_ERRORA("Error loading FBX file: %s", importer.GetErrorString());
		return false;
	}

	m_Skeleton.Create(m_fbxModel);

	m_Materials.resize(m_fbxModel->mNumMaterials);
	// fbx에 적용된 맵(텍스쳐) 들을 가져오는 과정.
	for (int i = 0; i < m_fbxModel->mNumMaterials; i++)
	{
		m_Materials[i].Create(m_fbxModel->mMaterials[i]);
	}

	// 각 애니메이션 벡터에 애니메이션 클립 생성하고 그 안에서 정보들 바인딩
	// ToDo 어떤 애니메이션을 플레이 하는지 비교해야함.
	for (int i = 0; i < m_fbxModel->mNumAnimations; i++)
	{
//		if (m_fbxModel->mAnimations[i] != nullptr)
//			m_Animation.Create(m_fbxModel->mAnimations[i]);
	}

	// vertex , index 정보 바인딩
	m_SkeletalMeshResources.resize(m_fbxModel->mNumMeshes); 
	for (int i = 0; i < m_fbxModel->mNumMeshes; i++)
	{
		m_SkeletalMeshResources[i].Create(m_fbxModel->mMeshes[i], &m_Skeleton);
		//	m_Meshes[i].CreateBoneWeightVertex(device, m_fbxModel->mMeshes[i]);
	}

	// 노드 순회 하면서 바인딩
	//m_RootNode.Create(this, m_fbxModel->mRootNode, &m_Animation);

	importer.FreeScene();

	return true;
}



//void SkeletalMeshSceneResource::AddAnimation(std::shared_ptr<AnimationResource> animation)
//{
//	m_Animations.push_back(animation);
//}

Material* SkeletalMeshSceneResource::GetMeshMaterial(UINT index)
{
	assert(index < m_Materials.size());
	UINT mindex = m_SkeletalMeshResources[index].m_MaterialIndex;
	assert(mindex < m_Materials.size());
	return &m_Materials[mindex];
}