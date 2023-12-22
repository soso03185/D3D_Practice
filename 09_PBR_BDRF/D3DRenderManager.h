#pragma once
#include "TutorialApp.h"

#include <DirectXtk/BufferHelpers.h>
#include <dxgi1_4.h>

namespace Math = DirectX::SimpleMath;

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
	Vector3 EyePosition = {};
	float pad;
};

struct CB_Marterial
{
	Math::Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };	// 16byte
	Math::Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };	// 16byte
	Math::Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };  // 16byte
	Math::Vector4 Emissive = { 1.0f,1.0f,1.0f,1.0f };  // 16byte
	float  SpecularPower = 80;					// 4
	int UseDiffuseMap = true;					// 4
	int UseNormalMap = true;					// 4
	int UseSpecularMap = true;                  // 4 16byte			
	int UseEmissiveMap = true;					// 4
	int UseOpacityMap = true;					// 4
	int m_pMetalnessRV = true;					// 4
	int m_pRoughnessRV = true;					// 4
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

class IRenderable
{
public:
	virtual void Render(ID3D11DeviceContext* pDeviceContext) = 0;
};

class IImGuiRenderable
{
public:
	virtual void ImGuiRender() = 0;
};

 class StaticMeshModel;
 class SkeletalMeshModel;
 class Material;

class D3DRenderManager
{
public:
	D3DRenderManager();
	~D3DRenderManager();

public:
	bool Initialize(HWND Handle, UINT Width, UINT Height);
	void Uninitialize();

	void Update();
	void Render();

	void CreateSkeletalMesh_VS_IL();
	void CreateStaticMesh_VS_IL();
	void CreatePS();
	void ApplyMaterial(Material* pMaterial);

	void GetVideoMemoryInfo(std::string& out);
	void GetSystemMemoryInfo(std::string& out);

	// 렌더링 이후에 목록은 사라진다.
	void AddMeshInstance(SkeletalMeshModel* pModel);
	void AddMeshInstance(StaticMeshModel* pModel);

public:
	static D3DRenderManager* Instance;
	ComPtr<IDXGIFactory4> m_pDXGIFactory;		// DXGI팩토리
	ComPtr<IDXGIAdapter3> m_pDXGIAdapter;		// 비디오카드 정보에 접근 가능한 인터페이스

	// 렌더링 파이프라인을 구성하는 필수 객체의 인터페이스 (  뎊스 스텐실 뷰도 있지만 아직 사용하지 않는다.)
	static ID3D11Device* m_pDevice;							// 디바이스	
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 즉시 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;					// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // 깊이값 처리
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// 텍스처 샘플러
	ID3D11BlendState* m_pAlphaBlendState = nullptr;		// 블렌드 상태 변경 (반투명처리를위한 블렌드상태)


	// 렌더링 파이프라인에 적용하는  객체와 정보
	ID3D11PixelShader* m_pPixelShader = nullptr;	// 픽셀 셰이더.	

	ID3D11VertexShader* m_pSkeletalMeshVertexShader = nullptr;	// SkeletalMesh 정점 셰이더.
	ID3D11InputLayout* m_pSkeletalMeshInputLayout = nullptr;	// SkeletalMesh 입력 레이아웃.

	ID3D11VertexShader* m_pStaticMeshVertexShader = nullptr;	// StaticMesh 정점 셰이더.
	ID3D11InputLayout* m_pStaticMeshInputLayout = nullptr;		// StaticMesh 입력 레이아웃.


	ID3D11Buffer* m_pConstantBuffer = nullptr;		// 상수 버퍼.
	ID3D11Buffer* m_pBoolBuffer = nullptr;		    // 상수 버퍼.
	ID3D11Buffer* m_pTransformBuffer = nullptr;		// 상수 버퍼.
	ID3D11Buffer* m_pLightBuffer = nullptr;		    // 상수 버퍼.
	ID3D11Buffer* m_pMatPalette = nullptr;		    // 상수 버퍼.
	ID3D11Buffer* m_pMaterialBuffer = nullptr;		    // 상수 버퍼.
	ConstantBuffer<CB_MatrixPalette> m_cbMatrixPallete; // DirectXTK의 상수버퍼 클래스 활용

	std::list<StaticMeshInstance*> m_StaticMeshInstance;		//  렌더링할 모델들의 포인터 저장해둔다. 
	std::list<SkeletalMeshInstance*> m_SkeletalMeshInstance;		//  렌더링할 모델들의 포인터 저장해둔다. 

	Model* m_pModel = nullptr;

	// 쉐이더에 에 전달할 데이터
	Matrix   m_World;		// 월드좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_View;		// 카메라좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_Projection;	// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.

	HWND m_hWnd = nullptr;
	D3D11_VIEWPORT viewport = {};

	CB_BoolBuffer CB_Bool;
	CB_TransformBuffer CB_Transform;
	CB_LightDirBuffer CB_Light;
	CB_Marterial CB_Mat;
	CB_MatrixPalette CB_MatPalette;

//------------------------------------

	//XMVECTOR m_Eye;
	//XMVECTOR m_At;
	//XMVECTOR m_Up;

	Vector3 m_CameraPos = Vector3(0.0f, 100.0f, -1000.0f);

	float m_Cam[3] = { 0.0f, -340.0f, 400.0f };
	float m_Scale = 1.0f;
	float m_Cb_speed = 1.3f;

	Vector3 m_Rotation = Vector3(0.0f, 0.0f, 0.0f);

	bool isAutoRotatePitch = false;
	bool isAutoRotateYaw = false;
	bool isAutoRotateRoll = false;

	float m_ClientWidth  = 500.0f;
	float m_ClientHeight = 500.0f;

private:
	IImGuiRenderable* m_pImGuiRender = nullptr;
};

