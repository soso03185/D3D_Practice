#pragma once
#include <d3d11.h>
#include <list>

class StaticMeshInstance;
class Material;

class D3DRenderManager
{
public:
	D3DRenderManager();
	~D3DRenderManager();
	void ApplyMaterial(Material* pMaterial);

	static D3DRenderManager* Instance;
	static ID3D11Device* m_pDevice;						// ����̽�
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ��� ����̽� ���ؽ�Ʈ
	ID3D11BlendState* m_pAlphaBlendState = nullptr;		// ���� ���� 

	ID3D11Buffer* m_pBoolBuffer = nullptr;		    // ��� ����.


	std::list<StaticMeshInstance*> m_StaticMeshInstance;		//  �������� �𵨵��� ������ �����صд�. 
};