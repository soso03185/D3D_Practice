
#include <assimp\mesh.h>

#include "Mesh.h"
#include "..\Common\Helper.h"

using namespace DirectX;

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

void Mesh::Create(ID3D11Device* device, aiMesh* mesh)
{
	m_MaterialIndex = mesh->mMaterialIndex;

	// 버텍스 정보 생성
	unique_ptr<Vertex[]> vertices(new Vertex[mesh->mNumVertices]);

	for (UINT i = 0; i < mesh -> mNumVertices; ++i)
	{
		vertices[i].Position = Vector4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
		vertices[i].Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		vertices[i].Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		vertices[i].TexCoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
	}
	
	/// CreateVertexBuffer ///
	D3D11_BUFFER_DESC vertexBD = {};
	vertexBD.ByteWidth = sizeof(Vertex) * mesh->mNumVertices;
	vertexBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBD.Usage = D3D11_USAGE_DEFAULT;
	vertexBD.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices.get();
	HR_T(device->CreateBuffer(&vertexBD, &vbData, &m_pVertexBuffer));

	// 버텍스 버퍼 정보
	m_VertexCount = mesh->mNumVertices;
	m_VertexBufferStride = sizeof(Vertex);
	m_VertexBufferOffset = 0;

	//=======================================================//

	// 인덱스 정보 생성
	unique_ptr<WORD[]> indices(new WORD[mesh->mNumFaces * 3]);

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
	indexBD.ByteWidth = sizeof(WORD) * mesh->mNumFaces * 3;
	indexBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBD.Usage = D3D11_USAGE_DEFAULT;
	indexBD.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices.get();
	HR_T(device->CreateBuffer(&indexBD, &ibData, &m_pIndexBuffer));
}