#pragma once
#include "TutorialApp.h"

class D3DRenderManager
{
public:
	D3DRenderManager();
	~D3DRenderManager();

	static D3DRenderManager* Instance;
	static ID3D11Device* m_pDevice;						// 디바이스

};