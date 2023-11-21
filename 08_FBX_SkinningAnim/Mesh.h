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
	Vector4 Position;    // 정점 위치 정보
	Vector2 TexCoord;
	Vector3 Normal;
	Vector3 Tangent;
};

struct BoneWeightVertex
{
	Vector4 Position;    // 정점 위치 정보
	Vector2 TexCoord;
	Vector3 Normal;
	Vector3 Tangent;

	// 영향받는 본 수는 최대 4개로 제한한다.
	// 참조하는 본의 인덱스의 범위는 최대 128개로 일단 처리하자.
	int BlendIndeces[4] = {};

	// 가중치 총합은 1이 되어야 한다.
	float BlendWeights[4] = {};

	void AddBoneData(int boneIndex, float weight)
	{
		// 적어도 하나는 데이터가 비어있어야 한다.
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
	UINT m_VertexBufferStride = 0;		// 버텍스 하나의 크기.
	UINT m_VertexBufferOffset = 0;		// 버텍스 버퍼의 오프셋.
	UINT m_IndexCount = 0;				// 인덱스 개수.
	UINT m_MaterialIndex = 0;			// 메테리얼 인덱스.
};