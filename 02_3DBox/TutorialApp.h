#pragma once
#include <windows.h>
#include "../Common/GameApp.h"
#include <d3d11.h>
#include <directxtk/SimpleMath.h>


using namespace DirectX::SimpleMath;
using namespace DirectX;

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


	// 렌더링 파이프라인에 적용하는  객체와 정보
	ID3D11VertexShader* m_pVertexShader = nullptr;	// 정점 셰이더.
	ID3D11PixelShader* m_pPixelShader = nullptr;	// 픽셀 셰이더.	
	ID3D11InputLayout* m_pInputLayout = nullptr;	// 입력 레이아웃.
	ID3D11Buffer* m_pVertexBuffer = nullptr;		// 버텍스 버퍼.
	UINT m_VertexBufferStride = 0;					// 버텍스 하나의 크기.
	UINT m_VertexBufferOffset = 0;					// 버텍스 버퍼의 오프셋.
	ID3D11Buffer* m_pIndexBuffer = nullptr;			// Cube 인덱스 버퍼.
	ID3D11Buffer* m_pIndexBuffer2 = nullptr;		// BG 인덱스 버퍼.
	int m_nIndices = 0;								// 인덱스 개수.
	int m_nIndices2 = 0;							// 인덱스 개수.
	ID3D11Buffer* m_pConstantBuffer = nullptr;		// 상수 버퍼.

	// data
	float m_Cb1_tr[3] = {};
	float m_Cb2_tr[3] = {3};
	float m_Cb3_tr[3] = {3};
	float m_Cb_speed[3] = {3};

	float m_Cam[3] = {};
	float m_EyeArr[3] = {};
	float m_AtArr[3] = {};
	float m_UpArr[3] = {};

	float m_Fov = {};
	float m_Near = {};
	float m_Far = {};

	XMVECTOR m_Eye;
	XMVECTOR m_At;
	XMVECTOR m_Up;

	// 쉐이더에 에 전달할 데이터
	Matrix   m_World1;		// 월드좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_World2;		// 월드좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_World3;		// 월드좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_World4;		// 월드좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_View;		// 카메라좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_Projection;	// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.

	D3D11_VIEWPORT viewport1 = {};
	D3D11_VIEWPORT viewport2 = {};

	// imgui
	Vector4 m_ClearColor = Vector4(0.45f, 0.55f, 0.60f, 1.00f);

	bool m_show_demo_window = true;
	float m_f;
	int m_counter;
	//

	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update();
	virtual void Render();

	void ImguiRender();
	void LeftRender();
	void RightRender();

	bool InitImGUI();
	void UninitImGUI();

	bool InitD3D();
	void UninitD3D();

	bool InitScene();		// 쉐이더,버텍스,인덱스
	void UninitScene();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

