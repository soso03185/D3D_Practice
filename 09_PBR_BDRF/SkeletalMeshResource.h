#pragma once
#include "SkeletonResource.h"
#include "Animation.h"
#include "Material.h"

struct BoneWeightVertex
{
	Math::Vector3 Position;		// ���� ��ġ ����.	
	Math::Vector2 TexCoord;
	Math::Vector3 Normal;
	Math::Vector3 Tangent;
	// ����޴� ������ �ִ�4���� �����Ѵ�.
	int BlendIndeces[4] = {};		// �����ϴ� ���� �ε����� ������ �ִ�128���� �ϴ�ó������
	float BlendWeights[4] = {};	// ����ġ ������ 1�� �Ǿ���Ѵ�.

	void AddBoneData(int boneIndex, float weight)
	{
		// ��� �ϳ��� �����Ͱ� ����־�� �Ѵ�.
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

struct BoneReference
{
	std::string NodeName;		 // ��� �̸�
	int BoneIndex = -1;			 // �� �ε���
	Math::Matrix OffsetMatrix;	 // ������ �޽��� OffsetMatrix
};



struct aiMesh;
class Node;

class SkeletalMeshResource
{
public:
	SkeletalMeshResource();
	~SkeletalMeshResource();

	std::vector<BoneWeightVertex>	m_Vertices;
	std::vector<WORD>				m_Indices;
	std::vector<BoneReference>		m_BoneReferences;

	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

	UINT m_VertexCount = 0;
	UINT m_VertexBufferStride = 0;						// ���ؽ� �ϳ��� ũ��.
	UINT m_VertexBufferOffset = 0;						// ���ؽ� ������ ������.
	UINT m_IndexCount = 0;				// �ε��� ����.
	UINT m_MaterialIndex = 0;			// ���׸��� �ε���.
	std::string m_Name;					// �޽� �̸�.	

	void Create(aiMesh* mesh, SkeletonResource* skeleton);
	void CreateVertexBuffer(BoneWeightVertex* vertices, UINT vertexCount);
	void CreateIndexBuffer(WORD* indies, UINT indexCount);
};

class SkeletalMeshSceneResource
{
public:
	SkeletalMeshSceneResource() {}
	~SkeletalMeshSceneResource() {}

	Math::Vector3 m_AABBmin;
	Math::Vector3 m_AABBmax;
	std::vector<SkeletalMeshResource> m_SkeletalMeshResources;
	std::vector<Material> m_Materials;
	SkeletonResource m_Skeleton;

	std::vector<std::shared_ptr<Animation>> m_Animations;

	bool Create(std::string filePath);

	Material* GetMeshMaterial(UINT index);

	void AddAnimation(std::shared_ptr<Animation> animation);
};