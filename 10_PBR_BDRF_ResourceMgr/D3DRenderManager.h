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
	static ID3D11Device* m_pDevice;						// 디바이스
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 즉시 디바이스 컨텍스트
	ID3D11BlendState* m_pAlphaBlendState = nullptr;		// 블렌드 상태 

	ID3D11Buffer* m_pBoolBuffer = nullptr;		    // 상수 버퍼.


	std::list<StaticMeshInstance*> m_StaticMeshInstance;		//  렌더링할 모델들의 포인터 저장해둔다. 
};