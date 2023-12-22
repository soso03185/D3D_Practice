#include "TutorialApp.h"
#include "../Common/Helper.h"
#include "../Common/GameApp.h"
#include "Model.h"
#include "Node.h"

#include <d3dcompiler.h>
#include <directxtk/WICTextureLoader.h>
// #include <directxtk\Mouse.h>
//#include <directxtk\Keyboard.h>

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

	if (!InitD3D())		return false;
	if (!InitImGUI())	return false;
	if (!InitScene())	return false;

	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_previousTime);
	QueryPerformanceCounter(&m_currentTime);

	return true;
}

void TutorialApp::Update()
{
}

void TutorialApp::ImguiRender()
{
	
}

void TutorialApp::ModelRender()
{
	//CB_MatrixPalette CB_MatPalatte;
	//m_pModel->m_Meshes[i].UpdateMatrixPalette(CB_MatPalatte.Array);
	//m_pDeviceContext->UpdateSubresource(m_pMatPalette, 0, nullptr, &CB_MatPalatte, 0, 0);
	//m_pDeviceContext->UpdateSubresource(m_pTransformBuffer, 0, nullptr, &CB_Transform, 0, 0);
	//m_pDeviceContext->UpdateSubresource(m_pBoolBuffer, 0, nullptr, &CB_Bool, 0, 0);

	//m_pDeviceContext->IASetIndexBuffer(m_pModel->m_Meshes[i].m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//m_pDeviceContext->IASetVertexBuffers
	//(
	//	0, 1,
	//	&m_pModel->m_Meshes[i].m_pVertexBuffer,
	//	//	&m_pModel->m_Meshes[i].m_pBWVertexBuffer,
	//	&m_pModel->m_Meshes[i].m_VertexBufferStride,
	//	&m_pModel->m_Meshes[i].m_VertexBufferOffset
	//);

	//m_pDeviceContext->DrawIndexed(m_pModel->m_Meshes[i].m_IndexCount, 0, 0);
}

void TutorialApp::Render()
{
	
}

bool TutorialApp::InitImGUI()
{
	
	return true;
}

void TutorialApp::UninitImGUI()
{

}

bool TutorialApp::InitD3D()
{
	// 결과값.
	HRESULT hr = 0;

	return true;
}

void TutorialApp::UninitD3D()
{

}

bool TutorialApp::InitScene()
{
	return true;
}

void TutorialApp::UninitScene()
{

	//  이거 하면 끌때 mesh 해제해주는거에서 오류생김
	//	delete m_pModel;
	//	delete m_pNode;

}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK TutorialApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}
