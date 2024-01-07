
#include <assimp\mesh.h>
#include <map>

#include "Mesh.h"
#include "D3DRenderManager.h"
#include "..\Common\Helper.h"

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

void Mesh::Create(aiMesh* mesh)
{
	m_MaterialIndex = mesh->mMaterialIndex;

	// 버텍스 정보 생성
	m_Vertices.resize(mesh->mNumVertices);

	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		m_Vertices[i].Position = Vector4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
		m_Vertices[i].TexCoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		m_Vertices[i].Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		m_Vertices[i].Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
	}
	
	CreateVertexBuffer<Vertex>(&m_Vertices[0], (UINT)m_Vertices.size(), &m_pVertexBuffer);

	// 인덱스 정보 생성
	unique_ptr<UINT[]> indices(new UINT[mesh->mNumFaces * 3]);

	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
		indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
	}

	CreateIndexBuffer(indices.get(), mesh->mNumFaces * 3);
}

void Mesh::CreateBoneWeight(aiMesh* mesh)
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
	CreateVertexBuffer<BoneWeightVertex>(&m_BoneWeightVertices[0], (UINT)m_BoneWeightVertices.size(), &m_pBWVertexBuffer);

	//=======================================================//
	// 인덱스 정보 생성
	unique_ptr<UINT[]> indices(new UINT[mesh->mNumFaces * 3]);

	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
		indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
	}

	CreateIndexBuffer(indices.get(), mesh->mNumFaces * 3);
}

void Mesh::CreateIndexBuffer(UINT* vertices, UINT vertexCount)
{
	/// CreateIndexBuffer  ///
	// 인덱스 개수 저장.
	m_IndexCount = vertexCount;

	D3D11_BUFFER_DESC indexBD = {};
	indexBD.ByteWidth = sizeof(UINT) * vertexCount;
	indexBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBD.Usage = D3D11_USAGE_DEFAULT;
	indexBD.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = vertices;
	HR_T(D3DRenderManager::m_pDevice->CreateBuffer(&indexBD, &ibData, &m_pIndexBuffer));
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