#pragma once
#include "../Common/Helper.h"

namespace Math = DirectX::SimpleMath;

class Mesh;
class Material;

class SkeletalMeshInstance
{
public:
	SkeletalMeshInstance();
	~SkeletalMeshInstance();

public:
	virtual void Render(ID3D11DeviceContext* deviceContext);

public:
	Mesh* m_pMeshResource = nullptr;
	Material* m_pMaterial = nullptr;
	Math::Matrix* m_pNodeWorldTransform = nullptr;		// SkeletalMesh의 월드행렬을 가진 노드의 포인터

};

