#pragma once
#include <windows.h>
#include "../Common/GameApp.h"
#include "D3DRenderManager.h"
#include "ResourceManager.h"

class TutorialApp :
	public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	ResourceManager ResMgr;  
	D3DRenderManager m_Renderer; 

	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update();
	virtual void Render();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

