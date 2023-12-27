#pragma once

#include <directxtk\SimpleMath.h>
#include <wrl.h>
#include <String> 
#include <memory>

#include "Material.h"
#include "Animation.h"
#include "SkeletonResource.h"

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace std;

struct Vertex
{
	Vector4 Position;    // ���� ��ġ ����
	Vector2 TexCoord;
	Vector3 Normal;
	Vector3 Tangent;
};

// Skinning ���� vertex �̰� �ٸ� ���� ��� ���� �׳� vertex�� ���.
struct BoneWeightVertex
{
	Vector4 Position;    // ���� ��ġ ����
	Vector2 TexCoord;
	Vector3 Normal;
	Vector3 Tangent;
	
	// �����ϴ� ���� �ε����� ������ �ִ� 128���� �ϴ� ó������. 
	// ���ؽ��� �����ϴ� ���� �ִ� 4���� �����ߴµ� �� ���� ���� Index
	int BlendIndeces[4] = {};  // ����޴� �� ���� �ִ� 4���� �����Ѵ�.
	
	// ���ؽ����� �����ϴ� ���� weight ��.
	// �ִ� 4���� �����ϴϱ� weight ���� 4������ �ְ� ��.
	float BlendWeights[4] = {};  // ����ġ ������ 1�� �Ǿ�� �Ѵ�.

	void AddBoneData(int boneIndex, float weight)
	{
		// ��� �ϳ��� �����Ͱ� ����־�� �Ѵ�.
		// 4�� �̻��� ���� ����� �־��� ���� �ڵ�.
		assert(BlendWeights[0] == 0.0f || BlendWeights[1] == 0.0f ||
				BlendWeights[2] == 0.0f || BlendWeights[3] == 0.0f);

		for (int i = 0; i < 4; i++)
		{
			if (BlendWeights[i] == 0.0f)  
			{
				BlendIndeces[i] = boneIndex;
				BlendWeights[i] = weight;
				return;
			}
		}
	}
};

struct Face
{
	UINT i0, i1, i2;
};

struct BoneReference
{
	string NodeName; // ��� �̸�
	Matrix OffsetMatrix;  // ������ �޽��� OffsetMatrix
	Matrix* NodeWorldMatrixPtr;
	int BoneIndex = -1;			 // �� �ε���
};

struct aiMesh;

class Mesh
{
public:
	Mesh();
	~Mesh();

public:
	void Create(aiMesh* mesh, SkeletonResource* skeleton);
	void CreateBoneWeightVertex(ID3D11Device* device, aiMesh* mesh);

	void UpdateMatrixPalette(Matrix* MatrixPalettePrt);
	void CreateBoneWeightVertexBuffer(ID3D11Device* device, BoneWeightVertex* boneWV, UINT size);
	
	template<typename T>
	void CreateVertexBuffer(T* vertices, UINT vertexCount);
	void CreateIndexBuffer(Face* faces, UINT faceCount);

	std::vector<Vertex>		m_Vertices;
	std::vector<Face>		m_Faces;
	std::string m_Name;					// �޽� �̸�.	

	ID3D11Buffer* m_pBWVertexBuffer;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	
	vector<BoneReference> m_BoneReferences;
    vector<BoneWeightVertex> m_BoneWeightVertices; 
	Matrix* m_pNodeWorld;

	UINT m_VertexCount = 0;
	UINT m_VertexBufferStride = 0;		// ���ؽ� �ϳ��� ũ��.
	UINT m_VertexBufferOffset = 0;		// ���ؽ� ������ ������.
	UINT m_IndexCount = 0;				// �ε��� ����.
	UINT m_MaterialIndex = 0;			// ���׸��� �ε���.
};

class SkeletalMeshSceneResource
{
public:
	SkeletalMeshSceneResource() {}
	~SkeletalMeshSceneResource() {}

	Vector3 m_AABBmin;
	Vector3 m_AABBmax;
	vector<Mesh> m_SkeletalMeshResources;
	vector<Material> m_Materials;
	SkeletonResource m_Skeleton;

	bool Create(string filePath);

	Material * GetMeshMaterial(UINT index);

//	vector<shared_ptr<AnimationResource>> m_Animations;

//	void AddAnimation(shared_ptr<AnimationResource> animation);
};

template<typename T>
inline void Mesh::CreateVertexBuffer(T* vertices, UINT vertexCount)
{
	/// CreateVertexBuffer ///
	D3D11_BUFFER_DESC vertexBD = {};
	vertexBD.ByteWidth = sizeof(T) * vertexCount;
	vertexBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBD.Usage = D3D11_USAGE_DEFAULT;
	vertexBD.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices;
	HR_T(D3DRenderManager::m_pDevice->CreateBuffer(&vertexBD, &vbData, &m_pVertexBuffer));

	// ���ؽ� ���� ����
	m_VertexCount = vertexCount;
	m_VertexBufferStride = sizeof(T);
	m_VertexBufferOffset = 0;
}
