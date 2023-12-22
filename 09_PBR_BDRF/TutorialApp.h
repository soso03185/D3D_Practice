#pragma once
#include <windows.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <wincodec.h>

#include "../Common/GameApp.h"
#include "Mesh.h"
#include "Material.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct aiNode;
struct aiAnimation;

class Model;
class Node;

class TutorialApp :
	public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	// data
	
	float m_EyeArr[3] = {};
	float m_AtArr[3] = {};
	float m_UpArr[3] = {};

	float m_vLightColor[3] = {1, 1, 1};
	float m_vLightDir[3] = {0, 0, 1};

	float m_Fov  = 100.0f;
	float m_Near = 0.01f;
	float m_Far  = 10000.0f;
	
	float m_SpecularPower = 4.0f;
	float m_Ambient = 0.369f;

	bool isNormalMap = true;
	bool isSpecularMap = true;
	bool isGamma = true;
	bool isDiffuse = true;
	bool isEmissive = true;
	bool isOpacity = true;
	bool isMetalness = true;
	bool isRoughness = true;

	// imgui
	Vector4 m_ClearColor = Vector4(0.45f, 0.55f, 0.60f, 1.00f);
	bool m_show_demo_window = true;
	float m_f;
	int m_counter;
	//

	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_previousTime;
	LARGE_INTEGER m_currentTime;
	float m_deltaTime = 0;

	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update();
	virtual void Render();

	void ImguiRender();
	void ModelRender();

	bool InitImGUI();
	void UninitImGUI();

	bool InitD3D();
	void UninitD3D();

	bool InitScene();		// Ω¶¿Ã¥ı,πˆ≈ÿΩ∫,¿Œµ¶Ω∫
	void UninitScene();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

