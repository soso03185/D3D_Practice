#pragma once
#include "Material.h"

// ���� ����.
struct Vertex
{
	Math::Vector3 Position;		// ���� ��ġ ����.	
	Math::Vector2 TexCoord;
	Math::Vector3 Normal;
	Math::Vector3 Tangent;
};


// �ϳ��� StaticMesh�� �ǹ��ϴ� ����������
class StaticMeshResource
{
public:
	StaticMeshResource();
	~StaticMeshResource();

	std::vector<Vertex>		m_Vertices;
	std::vector<WORD>		m_Indices;

	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

	UINT m_VertexCount = 0;
	UINT m_VertexBufferStride = 0;						// ���ؽ� �ϳ��� ũ��.
	UINT m_VertexBufferOffset = 0;						// ���ؽ� ������ ������.
	UINT m_IndexCount = 0;				// �ε��� ����.
	UINT m_MaterialIndex = 0;			// ���׸��� �ε���.
	std::string m_Name;					// �޽� �̸�.	

	Math::Vector3 m_AABBmin;
	Math::Vector3 m_AABBmax;

	void Create(aiMesh* mesh);
	void CreateVertexBuffer(Vertex* vertices, UINT vertexCount);
	void CreateIndexBuffer(WORD* indies, UINT indexCount);
	void GetAABB(Math::Vector3& min, Math::Vector3& max);
};

// �ϳ��� StaticMesh FBX�� �ǹ��ϴ� ����������, ���� �޽��� ���� �� �ִ�.
class StaticMeshSceneResource
{
public:
	StaticMeshSceneResource() {}
	~StaticMeshSceneResource() {}

	Math::Vector3 m_AABBmin;
	Math::Vector3 m_AABBmax;
	std::vector<StaticMeshResource> m_StaticMeshResources;
	std::vector<Material> m_Materials;

	bool Create(std::string filePath);
	Material* GetMeshMaterial(UINT index);
};