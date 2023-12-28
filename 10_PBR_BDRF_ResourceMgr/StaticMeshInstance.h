#pragma once
#include "../Common/Helper.h"

namespace Math = DirectX::SimpleMath;

class Mesh;
class Material;

// ���������� �׷����� Ŭ����.
class StaticMeshInstance
{
public:

	StaticMeshInstance();
	~StaticMeshInstance();

public:
	Mesh* m_pMeshResource = nullptr;
	Material* m_pMaterial = nullptr;
	Math::Matrix* m_pNodeWorldTransform = nullptr;		// StaticMesh�� ��������� ���� ����� ������

	void Create(Mesh* pMeshResource, Math::Matrix* pWorld, Material* pMaterial);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};
