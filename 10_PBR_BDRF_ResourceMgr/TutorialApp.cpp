#include "TutorialApp.h"
#include "../Common/Helper.h"
#include "../Common/GameApp.h"
#include "D3DRenderManager.h"
#include "ResourceManager.h"
#include "Model.h"

#include <d3dcompiler.h>
#include <directxtk/WICTextureLoader.h>
#include <imgui_impl_dx11.h>
// #include <directxtk\Mouse.h>
// #include <directxtk\Keyboard.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib,"d3dcompiler.lib")


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
			D3DRenderManager::Instance->IncreaseModel();
			//IncreaseModel();
		}
		else if (wParam == VK_DOWN) {
			//DecreaseModel();
		}
		break;
	}

	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}
