#pragma once
#include <directxtk\SimpleMath.h>
#include <wrl.h>
#include <d3d11.h>
#include <string>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

using namespace std;

//struct Vertex
//{
//	Vector4 Position;    // ���� ��ġ ����
//	Vector2 TexCoord;
//	Vector3 Normal;
//	Vector3 Tangent;
//};

struct BoneWeightVertex
{
	Vector4 Position;    // ���� ��ġ ����
	Vector2 TexCoord;
	Vector3 Normal;
	Vector3 Tangent;

	// ����޴� �� ���� �ִ� 4���� �����Ѵ�.
	int BlendIndeces[4] = {};  // �����ϴ� ���� �ε����� ������ �ִ� 128���� �ϴ� ó������.
	float BlendWeights[4] = {}; // ����ġ ������ 1�� �Ǿ���Ѵ�.

	void AddBoneData(int boneIndex, float weight)
	{
		// ��� �ϳ��� �����Ͱ� ����־�� �Ѵ�.
		assert(BlendWeights[0] = 0.0f || BlendWeights[1] == 0.0f ||
			BlendWeights[2] == 0.0f || BlendWeights[3] == 0.0f);

		for (int i = 0; i < 4; i++)  
		{
			if (BlendWeights[i] == 0.0f)
			{
				BlendWeights[i] = boneIndex;
				BlendWeights[i] = weight;
			}
		}
	}
};

struct BoneReference
{
	std::string m_NodeName;
	Matrix m_OffestMatrix;
	Matrix* m_NodeWorldMatrixPtr;
};

struct aiMesh;

class Mesh
{
public:
	Mesh();
	~Mesh();

public:
	void Create(ID3D11Device* device, aiMesh* mesh);
	void UpdateMatrixPallete(Matrix* MatrixPalletePtr);
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	
	Matrix* m_pNodeWorld;
	vector<BoneReference> m_BoneReferences;
	vector<BoneWeightVertex> m_BoneWeightVertices;

	UINT m_VertexCount = 0;
	UINT m_VertexBufferStride = 0;		// ���ؽ� �ϳ��� ũ��.
	UINT m_VertexBufferOffset = 0;		// ���ؽ� ������ ������.
	UINT m_IndexCount = 0;				// �ε��� ����.
	UINT m_MaterialIndex = 0;			// ���׸��� �ε���.
};