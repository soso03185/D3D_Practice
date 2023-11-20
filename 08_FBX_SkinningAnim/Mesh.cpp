
#include <assimp\mesh.h>

#include "Mesh.h"
#include "..\Common\Helper.h"
#include <map>

using namespace DirectX;

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pBoneWeightVertexBuffer);
}


void Mesh::Create(ID3D11Device* device, aiMesh* mesh)
{
	m_MaterialIndex = mesh->mMaterialIndex;
	
	// ���ؽ� ���� ����
	unique_ptr<BoneWeightVertex[]> vertices(new BoneWeightVertex[mesh->mNumVertices]);
	
	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		Vector4 originalPosition(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
		Matrix m_pNodeWorldMatrix = *m_pNodeWorld; // ���ϴ� ��� ������ �ʱ�ȭ
		Vector4 transformedPosition = Vector4::Transform(originalPosition, m_pNodeWorldMatrix);

		vertices[i].Position = originalPosition;
		vertices[i].TexCoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertices[i].Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		vertices[i].Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
	}

	/// CreateVertexBuffer ///
	D3D11_BUFFER_DESC vertexBD = {};
	vertexBD.ByteWidth = sizeof(BoneWeightVertex) * mesh->mNumVertices;
	vertexBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBD.Usage = D3D11_USAGE_DEFAULT;
	vertexBD.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices.get();
	HR_T(device->CreateBuffer(&vertexBD, &vbData, &m_pVertexBuffer));

	// ���ؽ� ���� ����
	m_VertexCount = mesh->mNumVertices;
	m_VertexBufferStride = sizeof(BoneWeightVertex);
	m_VertexBufferOffset = 0;

	//=======================================================//

	// �ε��� ���� ����
	unique_ptr<WORD[]> indices(new WORD[mesh->mNumFaces * 3]);

	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
		indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
	}

	/// CreateIndexBuffer  ///
	// �ε��� ���� ����.
	m_IndexCount = mesh->mNumFaces * 3;

	D3D11_BUFFER_DESC indexBD = {};
	indexBD.ByteWidth = sizeof(WORD) * mesh->mNumFaces * 3;
	indexBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBD.Usage = D3D11_USAGE_DEFAULT;
	indexBD.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices.get();
	HR_T(device->CreateBuffer(&indexBD, &ibData, &m_pIndexBuffer));

	BindingBone(device, mesh);
}

void Mesh::BindingBone(ID3D11Device* device, aiMesh* mesh)
{
	UINT meshBoneCount = mesh->mNumBones;   // �޽��� ����� �� ����
	m_BoneReferences.resize(meshBoneCount); // �� ���� ���� �����̳� ũ�� ����

	// �޽��� ����� ������ ó��
	UINT boneIndexCounter = 0;
	std::map<std::string, int> BoneMapping;

	for (UINT i = 0; i < meshBoneCount; ++i)
	{
		aiBone* bone = mesh->mBones[i];
		std::string boneName = bone->mName.C_Str();

		UINT boneIndex = 0;
		if (BoneMapping.find(boneName) == BoneMapping.end())
		{
			// Map bone name to bone Index
			boneIndex = boneIndexCounter;
			boneIndexCounter++;
			m_BoneReferences[boneIndex].m_NodeName = boneName;
			m_BoneReferences[boneIndex].m_OffestMatrix = Matrix(&bone->mOffsetMatrix.a1).Transpose();
			BoneMapping[boneName] = boneIndex;
		}
		else
			boneIndex = BoneMapping[boneName];

		// ���� ����� ���ؽ����� ó��
		for (UINT j = 0; j < bone->mNumWeights; ++j)
		{
			UINT vertexID = bone->mWeights[i].mVertexId;
			float weight = bone->mWeights[i].mWeight;
			m_BoneWeightVertices[vertexID].AddBoneData(boneIndex, weight);
		}
	}

	CreateBoneWeightVertexBuffer(device, &m_BoneWeightVertices[0], (UINT)m_BoneWeightVertices.size());
}

void Mesh::CreateBoneWeightVertexBuffer(ID3D11Device* device, BoneWeightVertex* vertices, UINT numVertices)
{
	// Create Bone Weight Vertex Buffer
	D3D11_BUFFER_DESC vertexBD = {};
	vertexBD.ByteWidth = sizeof(BoneWeightVertex) * numVertices;
	vertexBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBD.Usage = D3D11_USAGE_DEFAULT;
	vertexBD.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices;

	HR_T(device->CreateBuffer(&vertexBD, &vbData, &m_pBoneWeightVertexBuffer));
}

void Mesh::UpdateMatrixPallete(Matrix* MatrixPalletePtr)
{
	assert(m_BoneReferences.size() < 128);
	for (UINT i = 0; i < m_BoneReferences.size(); ++i)
	{
		Matrix& BoneNodeWorldMatrix = *m_BoneReferences[i].m_NodeWorldMatrixPtr;

		// HLSL ��� ���ۿ� ������Ʈ�� ��, �ٷ� ������ �� �ֵ��� ��ġ�ؼ� �����Ѵ�.
		MatrixPalletePtr[i] = (m_BoneReferences[i].m_OffestMatrix * BoneNodeWorldMatrix).Transpose();
	}
}
