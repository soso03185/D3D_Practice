#pragma once
#include "../Common/Helper.h"

namespace Math = DirectX::SimpleMath;

class Mesh;
class Material;
class Node;
struct CB_MatrixPalette;

class SkeletalMeshInstance
{
public:
	SkeletalMeshInstance();
	~SkeletalMeshInstance();

public:
	Mesh* m_pMeshResource = nullptr;
	Material* m_pMaterial = nullptr;
	Math::Matrix* m_pNodeWorldTransform = nullptr;		// SkeletalMesh�� ��������� ���� ����� ������
	std::vector<Math::Matrix*>		m_BoneReferences;	// ���� �ش�Ǵ� ���MATRIX�� �����͸� �����Ѵ�.

public:
	void Create(Mesh* pMeshResource, Material* pMaterial, Node* pRootNode);
	virtual void Render(ID3D11DeviceContext* deviceContext);
	void UpdateMatrixPallete(CB_MatrixPalette* pMatrixPallete);
};

