#pragma once
#include <directxtk\SimpleMath.h>
#include <wrl.h>
#include <d3d11.h>
#include <String> 

#include "Material.h"

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

struct BoneReference
{
	std::string NodeName; // ��� �̸�
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
	void Create(aiMesh* mesh);
	void CreateBoneWeightVertex(aiMesh* mesh);
	void CreateBoneWeightVertexBuffer(BoneWeightVertex* boneWV, UINT size);

	void UpdateMatrixPalette(Matrix* MatrixPalettePrt);

	std::vector<BoneWeightVertex>	m_BoneWeightVertices;
	std::vector<Vertex>				m_Vertices;

	ID3D11Buffer* m_pBWVertexBuffer;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;

	vector<BoneReference> m_BoneReferences;
	Matrix* m_pNodeWorld;

	UINT m_VertexCount = 0;
	UINT m_VertexBufferStride = 0;		// ���ؽ� �ϳ��� ũ��.
	UINT m_VertexBufferOffset = 0;		// ���ؽ� ������ ������.
	UINT m_IndexCount = 0;				// �ε��� ����.
	UINT m_MaterialIndex = 0;			// ���׸��� �ε���.
};
