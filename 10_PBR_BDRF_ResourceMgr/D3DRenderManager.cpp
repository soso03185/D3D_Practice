
#include "D3DRenderManager.h"

D3DRenderManager* D3DRenderManager::Instance = nullptr;
ID3D11Device* D3DRenderManager::m_pDevice = nullptr;


D3DRenderManager::D3DRenderManager()
{
	assert(Instance == nullptr);
	Instance = this;
}

D3DRenderManager::~D3DRenderManager()
{
//	Uninitialize();
}

