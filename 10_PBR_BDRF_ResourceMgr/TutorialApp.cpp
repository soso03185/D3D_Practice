#include "TutorialApp.h"
#include "../Common/Helper.h"
#include "../Common/GameApp.h"
#include "Model.h"
#include "Node.h"
#include "D3DRenderManager.h"
#include "ResourceManager.h"

#include <d3dcompiler.h>
#include <directxtk/WICTextureLoader.h>
// #include <directxtk\Mouse.h>
// #include <directxtk\Keyboard.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib,"d3dcompiler.lib")

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp/scene.h>


TutorialApp::TutorialApp(HINSTANCE hInstance)
	:GameApp(hInstance)
{
}

TutorialApp::~TutorialApp()
{
	UninitScene();
	UninitD3D();
	UninitImGUI();
}

bool TutorialApp::Initialize(UINT Width, UINT Height)
{
	__super::Initialize(Width, Height);

	D3DRenderManager::Instance->Initialize(Width,Height,m_hWnd);
	m_pDevice = D3DRenderManager::Instance->m_pDevice;
	m_pDeviceContext = D3DRenderManager::Instance->m_pDeviceContext;

	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_previousTime);
	QueryPerformanceCounter(&m_currentTime);

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

void TutorialApp::UninitImGUI()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void TutorialApp::UninitD3D()
{
	// Cleanup DirectX
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pRenderTargetView);
}

void TutorialApp::UninitScene()
{
	//  이거 하면 끌때 mesh 해제해주는거에서 오류생김
	//	delete m_pModel;
	//	delete m_pNode;

	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pBoolBuffer);
	SAFE_RELEASE(m_pTransformBuffer);
	SAFE_RELEASE(m_pLightBuffer);
	SAFE_RELEASE(m_pAlphaBlendState);
	SAFE_RELEASE(m_pMatPalette);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK TutorialApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_UP) {		
			D3DRenderManager::Instance->m_pModel->ReadFile("../Resource/zeldaPosed001.fbx");
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
