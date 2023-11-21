#pragma once
#include <directxtk\SimpleMath.h>
#include <wrl.h>
#include <d3d11.h>
#include <String>

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

struct BoneWeightVertex
{
	Vector4 Position;    // ���� ��ġ ����
	Vector2 TexCoord;
	Vector3 Normal;
	Vector3 Tangent;

	// ����޴� �� ���� �ִ� 4���� �����Ѵ�.
	// �����ϴ� ���� �ε����� ������ �ִ� 128���� �ϴ� ó������.
	int BlendIndeces[4] = {};

	// ����ġ ������ 1�� �Ǿ�� �Ѵ�.
	float BlendWeights[4] = {};

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
	std::string NodeName;
	Matrix OffsetMatrix;
	Matrix* NodeWorldMatrixPtr;
};

struct aiMesh;

class Mesh
{
public:
	Mesh();
	~Mesh();

public:
	void Create(ID3D11Device* device, aiMesh* mesh);
	void BindBone(ID3D11Device* device, aiMesh* mesh);
	void UpdateMatrixPalette(Matrix* MatrixPalettePrt);

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	
//	vector<WORD> Indices;
	vector<BoneReference> m_BoneReferences;
	vector<BoneWeightVertex> m_BoneWeightVertices;

	Matrix* m_pNodeWorld;

	UINT m_VertexCount = 0;
	UINT m_VertexBufferStride = 0;		// ���ؽ� �ϳ��� ũ��.
	UINT m_VertexBufferOffset = 0;		// ���ؽ� ������ ������.
	UINT m_IndexCount = 0;				// �ε��� ����.
	UINT m_MaterialIndex = 0;			// ���׸��� �ε���.
};