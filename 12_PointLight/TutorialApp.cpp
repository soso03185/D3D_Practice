#include "TutorialApp.h"
#include "../Common/Helper.h"
#include "../Common/GameApp.h"
#include "D3DRenderManager.h"
#include "ResourceManager.h"
#include "ModelResource.h"

#include <d3dcompiler.h>
#include <directxtk/WICTextureLoader.h>
#include <imgui_impl_dx11.h> 
// #include <directxtk\Mouse.h>
// #include <directxtk\Keyboard.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "DXGI.lib")  // for  CreateDXGIFactory1 ÇÔ¼ö

TutorialApp::TutorialApp(HINSTANCE hInstance)
	:GameApp(hInstance)
{
}

TutorialApp::~TutorialApp()
{
}

bool TutorialApp::Initialize(UINT Width, UINT Height)
{
	__super::Initialize(Width, Height);

	D3DRenderManager::Instance->Initialize(Width,Height,m_hWnd);
	return true;
}

void TutorialApp::Update()
{
	__super::Update();

	float t = GameTimer::m_Instance->TotalTime() / 4;
	m_Renderer.Update();
}

void TutorialApp::Render()
{
	m_Renderer.Render();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK TutorialApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_UP) {
			//D3DRenderManager::Instance->IncreaseStaticModel("../Resource/Primrose_Egypt.fbx");
			D3DRenderManager::Instance->IncreaseSkeletalModel("../Resource/SkinningTest.fbx");
			//D3DRenderManager::Instance->IncreaseSkeletalModel("../Resource/Zombie_Run.fbx");
			//D3DRenderManager::Instance->IncreaseStaticModel("../Resource/sphere_alpha1.fbx");
			D3DRenderManager::Instance->IncreaseStaticModel("../Resource/zeldaPosed001.fbx");
			//D3DRenderManager::Instance->IncreaseStaticModel("../Resource/cerberus2.fbx");
		}
		else if (wParam == VK_DOWN) {
			//D3DRenderManager::Instance->DecreaseModel();
			D3DRenderManager::Instance->ChangeAnimation(0);
		}
		else if (wParam == VK_RIGHT)
		{
			D3DRenderManager::Instance->ChangeAnimation(2);
		}
		else if (wParam == VK_LEFT)
		{
			D3DRenderManager::Instance->ChangeAnimation(1);
		}
		break;
	}

	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}

