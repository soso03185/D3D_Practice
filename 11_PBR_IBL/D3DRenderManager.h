#pragma once
#include <d3d11.h>
#include <list>
#include <DirectXtk/BufferHelpers.h>
#include <directxtk/SimpleMath.h>
#include <string>
#include <dxgi1_4.h>
#include <memory>


using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace DirectX;
namespace Math = DirectX::SimpleMath;

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

struct CB_TransformW
{
	Matrix mWorld;
};
struct CB_TransformVP
{
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

struct CB_IBL
{
	int UseIBL = false;							// 4  16byte
	float AmbientOcclusion = 1.0f;				// 4
	Vector2 pad;								// 12	16
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


class EnvironmentMeshComponent;
class SkeletalMeshInstance;
class SkeletalMeshComponent;
class StaticMeshComponent;
class StaticMeshInstance;
class Material;


class D3DRenderManager
{
public:
	D3DRenderManager();
	~D3DRenderManager();

public:
	//  Need  //
	bool Initialize(UINT Width, UINT Height, HWND hWnd);
	bool InitImGUI();
	bool InitD3D();
	void InitScene();
	void Uninitialize();

	//   Main Loop   //
	void Update();
	void Render();

	//  Utility  //
	void IncreaseStaticModel(std::string pilePath);
	void IncreaseSkeletalModel(std::string pilePath);
	void DecreaseModel();
	void ApplyMaterial(Material* pMaterial);
	void AddMeshInstance(StaticMeshComponent* pModel);
	void AddMeshInstance(SkeletalMeshComponent* pModel);
	void ConstantBuffUpdate();
	void SetEnvironment(std::weak_ptr<EnvironmentMeshComponent> val);

	void CreateIBL();
private:
	//?   CREATE	  ///
	void CreateVS_IL();
	void CreatePS();
	void CreateEnvironment();

	void CreateSamplerState();
	void CreateBlendState();
	void CreateConstantBuffer();
	void CreateSwapChain();
	void CreateStencilAndDepth();
	void CreateRenderTargetView();

	//?   Render    ///
	void ImguiRender();
	void RenderStaticMeshInstance();
	void RenderSkeletalMeshInstance();
	void RenderEnvironment();

	//?   Util     ///
	void GetVideoMemoryInfo(std::string& out);
	void GetSystemMemoryInfo(std::string& out);


public: 
	///   D3D   ///
	static D3DRenderManager* Instance;
	static ID3D11Device* m_pDevice;					 // 디바이스
	
	ID3D11DeviceContext* m_pDeviceContext = nullptr; // 즉시 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;					// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // 깊이값 처리
	ComPtr<IDXGIFactory4> m_pDXGIFactory;		// DXGI팩토리
	ComPtr<IDXGIAdapter3> m_pDXGIAdapter;		// 비디오카드 정보에 접근 가능한 인터페이스
	UINT m_ClientWidth;
	UINT m_ClientHeight;
	HWND m_hWnd;


public:
	//?   CB Data   ///
	ID3D11Buffer* m_pBoolBuffer = nullptr;		    // 상수 버퍼.
	ID3D11Buffer* m_pLightBuffer = nullptr;		    // 상수 버퍼.
	ID3D11Buffer* m_pMatPalette = nullptr;		    // 상수 버퍼.
	ID3D11Buffer* m_pIBL_Buffer = nullptr;			// 상수 버퍼.
	ID3D11Buffer* m_pTransformW_Buffer = nullptr;		// 상수 버퍼.
	ID3D11Buffer* m_pTransformVP_Buffer = nullptr;		// 상수 버퍼.

	CB_TransformW m_TransformW;
	CB_TransformVP m_TransformVP;
	CB_IBL m_IBL;


	///   CONTAINER   ///
	std::list<StaticMeshInstance*> m_StaticMeshInstance;		//  렌더링할 모델들의 포인터 저장해둔다. 
	std::list<StaticMeshComponent*> m_StaticMeshComponents;		//  렌더링할 모델들의 포인터 저장해둔다.
	std::list<SkeletalMeshInstance*> m_SkeletalMeshInstance;		//  렌더링할 모델들의 포인터 저장해둔다.
	std::list<SkeletalMeshComponent*> m_SkeletalMeshComponents;		//  렌더링할 모델들의 포인터 저장해둔다.

	std::weak_ptr<EnvironmentMeshComponent> m_pEnvironmentMeshComponent;

	///   FOR RENDERING   ///
	ID3D11VertexShader* m_pEnvironmentVertexShader = nullptr; // Environment 정점 셰이더.
	ID3D11PixelShader*  m_pEnvironmentPixelShader = nullptr;	// Environment 정점 셰이더.

	ID3D11VertexShader* m_pStaticVertexShader = nullptr;	// 정점 셰이더.
	ID3D11VertexShader* m_pSkeletalVertexShader = nullptr;	// 정점 셰이더.
	ID3D11PixelShader*  m_pPixelShader = nullptr;	// 픽셀 셰이더.	
 
 	ID3D11InputLayout* m_pStaticInputLayout = nullptr;	// 입력 레이아웃.
	ID3D11InputLayout* m_pSkeletalInputLayout = nullptr;	// 입력 레이아웃.

	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// 텍스처 샘플러
	ID3D11SamplerState* m_pSamplerClamp = nullptr;		// 텍스처 샘플러
	ID3D11BlendState*   m_pAlphaBlendState = nullptr;		// 블렌드 상태 변경 (반투명처리를위한 블렌드상태)
		
	///  FOR SHADER  ///
	XMVECTOR m_Eye;
	XMVECTOR m_At;
	XMVECTOR m_Up;
	Matrix   m_View;		// 카메라좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_Projection;	// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.

	D3D11_VIEWPORT viewport = {};
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_previousTime;
	LARGE_INTEGER m_currentTime;
	float m_deltaTime = 0;

	float m_Cam[3] = { 0.0f, 0.0f, -500.0f };
	float m_Fov = 100.0f;
	float m_Near = 0.01f;
	float m_Far = 10000.0f;

	float m_SpecularPower = 4.0f;
	float m_Ambient = 0.369f;

	float m_vLightColor[3] = { 1, 1, 1 };
	float m_vLightDir[3] = { 0, 0, 1 };

	bool isNormalMap = true;
	bool isSpecularMap = true;
	bool isGamma = true;
	bool isDiffuse = true;
	bool isEmissive = true;
	bool isOpacity = true;
	bool isMetalness = true;
	bool isRoughness = true;
};
