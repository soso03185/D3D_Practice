#pragma once
#include "../Common/Helper.h"

namespace Math = DirectX::SimpleMath;

class Mesh;
class Material;

// 최종적으로 그려지는 클래스.
class StaticMeshInstance
{
public:

	StaticMeshInstance();
	~StaticMeshInstance();

public:
	Mesh* m_pMeshResource = nullptr;
	Material* m_pMaterial = nullptr;
	Math::Matrix* m_pNodeWorldTransform = nullptr;		// StaticMesh의 월드행렬을 가진 노드의 포인터

	void Create(Mesh* pMeshResource, Math::Matrix* pWorld, Material* pMaterial);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};
