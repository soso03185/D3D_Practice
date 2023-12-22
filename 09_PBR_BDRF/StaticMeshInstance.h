#pragma once


// �������� �ʿ��� ��������, ���� �޽��� ����� Transform, ����� ���͸���
class StaticMeshResource;
class Node;
class Material;
class StaticMeshInstance
{
public:
	StaticMeshInstance();
	~StaticMeshInstance();
public:
	StaticMeshResource* m_pMeshResource = nullptr;
	Material* m_pMaterial = nullptr;
	Math::Matrix* m_pNodeWorldTransform = nullptr;		// StaticMesh�� ��������� ���� ����� ������

	void Create(StaticMeshResource* pMeshResource, Node* pRootNode, Material* pMaterial);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};