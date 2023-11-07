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

	// 렌더링 파이프라인을 구성하는 필수 객체의 인터페이스 (  뎊스 스텐실 뷰도 있지만 아직 사용하지 않는다.)
	ID3D11Device* m_pDevice = nullptr;						// 디바이스	
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 즉시 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;					// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // 깊이값 처리
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// 텍스처 샘플러
	ID3D11BlendState* m_pAlphaBlendState = nullptr;		// 블렌드 상태 변경 (반투명처리를위한 블렌드상태)

	// 렌더링 파이프라인에 적용하는  객체와 정보
	ID3D11VertexShader* m_pVertexShader = nullptr;	// 정점 셰이더.
	ID3D11PixelShader* m_pPixelShader = nullptr;	// 픽셀 셰이더.	
	ID3D11InputLayout* m_pInputLayout = nullptr;	// 입력 레이아웃.


	ID3D11Buffer* m_pConstantBuffer = nullptr;		// 상수 버퍼.
	ID3D11Buffer* m_pBoolBuffer = nullptr;		    // 상수 버퍼.
	ID3D11Buffer* m_pTransformBuffer = nullptr;		// 상수 버퍼.
	ID3D11Buffer* m_pLightBuffer = nullptr;		    // 상수 버퍼.

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

	// 쉐이더에 에 전달할 데이터
	Matrix   m_World;		// 월드좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_View;		// 카메라좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_Projection;	// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.

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

	bool InitScene();		// 쉐이더,버텍스,인덱스
	void UninitScene();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

