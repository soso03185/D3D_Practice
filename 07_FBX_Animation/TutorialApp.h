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

	// ������ ������������ �����ϴ� �ʼ� ��ü�� �������̽� (  �X�� ���ٽ� �䵵 ������ ���� ������� �ʴ´�.)
	ID3D11Device* m_pDevice = nullptr;						// ����̽�	
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ��� ����̽� ���ؽ�Ʈ
	IDXGISwapChain* m_pSwapChain = nullptr;					// ����ü��
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// ������ Ÿ�ٺ�
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // ���̰� ó��
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// �ؽ�ó ���÷�
	ID3D11BlendState* m_pAlphaBlendState = nullptr;		// ���� ���� ���� (������ó�������� �������)

	// ������ ���������ο� �����ϴ�  ��ü�� ����
	ID3D11VertexShader* m_pVertexShader = nullptr;	// ���� ���̴�.
	ID3D11PixelShader* m_pPixelShader = nullptr;	// �ȼ� ���̴�.	
	ID3D11InputLayout* m_pInputLayout = nullptr;	// �Է� ���̾ƿ�.


	ID3D11Buffer* m_pConstantBuffer = nullptr;		// ��� ����.
	ID3D11Buffer* m_pBoolBuffer = nullptr;		    // ��� ����.
	ID3D11Buffer* m_pTransformBuffer = nullptr;		// ��� ����.
	ID3D11Buffer* m_pLightBuffer = nullptr;		    // ��� ����.

	vector<Mesh> m_Meshes;
	vector<Material> m_Materials;

	Node* m_pNode = nullptr;
	Model* m_pModel = nullptr;

	// data
	float m_Scale = 1.0f;
	float m_Cb_Trans[3] = {0, -350.f, 500.0f};
	float m_Cb_Rot[3] = { 0 };
	float m_Cb_speed[3] = { 3 };

	float m_Cam[3] = { 0, 0, -7.0f };
	float m_EyeArr[3] = {};
	float m_AtArr[3] = {};
	float m_UpArr[3] = {};

	float m_vLightColor[3] = {1, 1, 1};
	float m_vLightDir[3] = {0, 0, 1};

	float m_Fov  = 100.0f;
	float m_Near = 0.01f;
	float m_Far  = 10000.0f;
	
	float m_SpecularPower = 4.0f;
	float m_Ambient = 0.1f;


	bool isAutoRotatePitch = false;
	bool isAutoRotateYaw = false;
	bool isAutoRotateRoll = false;

	bool isNormalMap = true;
	bool isSpecularMap = true;
	bool isGamma = true;
	bool isDiffuse = true;
	bool isEmissive = true;
	bool isOpacity = true;


	XMVECTOR m_Eye;
	XMVECTOR m_At;
	XMVECTOR m_Up;

	// ���̴��� �� ������ ������
	Matrix   m_World;		// ������ǥ�� �������� ��ȯ�� ���� ���.
	Matrix   m_View;		// ī�޶���ǥ�� �������� ��ȯ�� ���� ���.
	Matrix   m_Projection;	// ������ġ��ǥ��( Normalized Device Coordinate) �������� ��ȯ�� ���� ���.

	D3D11_VIEWPORT viewport = {};

	// imgui
	Vector4 m_ClearColor = Vector4(0.45f, 0.55f, 0.60f, 1.00f);
	bool m_show_demo_window = true;
	float m_f;
	int m_counter;


	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update();
	virtual void Render();

	void ImguiRender();
	void CubeRender();

	bool InitImGUI();
	void UninitImGUI();

	bool InitD3D();
	void UninitD3D();

	bool InitScene();		// ���̴�,���ؽ�,�ε���
	void UninitScene();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

