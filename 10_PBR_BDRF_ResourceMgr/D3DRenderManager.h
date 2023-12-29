#pragma once
#include <d3d11.h>
#include <list>
#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct CB_ConstantBuffer
{
	float mSpecularPower;
	float mAmbient;
	float gpadding[2];
};

struct CB_BoolBuffer
{
	int UseNormalMap;
	int UseSpecularMap;
	int UseGamma;
	int UseDiffuseMap;

	int UseEmissiveMap;
	int UseOpacityMap;
	int UseMetalnessMap;
	int UseRoughnessMap;
};

struct CB_TransformBuffer
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
};

struct CB_LightDirBuffer
{
	Vector4 vLightDir;
	Vector4 vLightColor;
	Vector4 mWorldCameraPosition;
	Vector4 pad[1];
};

// 버텍스 셰이더에 옮겨주기 위해 사용하는, 휘발성 저장 공간.
// 메쉬가 사용하고 그 다음 메쉬가 사용하고 반복...하기때문에 
// 휘발성 저장 공간이 하나만 있어도 됨.
// 
// 여기에 저장되는 것은. 메쉬를 구성하는 버텍스들이 참조하는 모든 bone들의 offset * boneWorld 값들.
// 판단은 bone의 name이나 index로 같은지 판단해서 사용함.
struct CB_MatrixPalette
{
	Matrix Array[128];
};



class StaticMeshComponent;
class StaticMeshInstance;
class Material;
class Model;

class D3DRenderManager
{
public:
	D3DRenderManager();
	~D3DRenderManager();


	//  Init  //
public:
	bool Initialize(UINT Width, UINT Height, HWND& hWnd);
	bool InitD3D();
	bool InitImGUI();
	bool InitScene();


	//  Utility  //
public:
	void ApplyMaterial(Material* pMaterial);



	// Public Data //
public:
	UINT m_ClientWidth;
	UINT m_ClientHeight;
	HWND m_hWnd;

	static D3DRenderManager* Instance;
	static ID3D11Device* m_pDevice;					 // 디바이스
	ID3D11DeviceContext* m_pDeviceContext = nullptr; // 즉시 디바이스 컨텍스트




	//  Data  //
public: 
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
	ID3D11Buffer* m_pMatPalette = nullptr;		    // 상수 버퍼.

	XMVECTOR m_Eye;
	XMVECTOR m_At;
	XMVECTOR m_Up;

	// 쉐이더에 에 전달할 데이터
	Matrix   m_World;		// 월드좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_View;		// 카메라좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_Projection;	// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.

	D3D11_VIEWPORT viewport = {};


	float m_Cam[3] = { 0.0f, -340.0f, 400.0f };


	//Node* m_pNode = nullptr;
	Model* m_pModel = nullptr;

	std::list<StaticMeshInstance*> m_StaticMeshInstance;		//  렌더링할 모델들의 포인터 저장해둔다. 
	std::list<StaticMeshComponent*> m_StaticMeshComponents;		//  렌더링할 모델들의 포인터 저장해둔다.

};