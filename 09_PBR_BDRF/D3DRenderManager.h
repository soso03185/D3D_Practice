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

// ���ؽ� ���̴��� �Ű��ֱ� ���� ����ϴ�, �ֹ߼� ���� ����.
// �޽��� ����ϰ� �� ���� �޽��� ����ϰ� �ݺ�...�ϱ⶧���� 
// �ֹ߼� ���� ������ �ϳ��� �־ ��.
// 
// ���⿡ ����Ǵ� ����. �޽��� �����ϴ� ���ؽ����� �����ϴ� ��� bone���� offset * boneWorld ����.
// �Ǵ��� bone�� name�̳� index�� ������ �Ǵ��ؼ� �����.
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

	// ������ ���Ŀ� ����� �������.
	void AddMeshInstance(SkeletalMeshModel* pModel);
	void AddMeshInstance(StaticMeshModel* pModel);

public:
	static D3DRenderManager* Instance;
	ComPtr<IDXGIFactory4> m_pDXGIFactory;		// DXGI���丮
	ComPtr<IDXGIAdapter3> m_pDXGIAdapter;		// ����ī�� ������ ���� ������ �������̽�

	// ������ ������������ �����ϴ� �ʼ� ��ü�� �������̽� (  �X�� ���ٽ� �䵵 ������ ���� ������� �ʴ´�.)
	static ID3D11Device* m_pDevice;							// ����̽�	
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ��� ����̽� ���ؽ�Ʈ
	IDXGISwapChain* m_pSwapChain = nullptr;					// ����ü��
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// ������ Ÿ�ٺ�
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // ���̰� ó��
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// �ؽ�ó ���÷�
	ID3D11BlendState* m_pAlphaBlendState = nullptr;		// ���� ���� ���� (������ó�������� �������)


	// ������ ���������ο� �����ϴ�  ��ü�� ����
	ID3D11PixelShader* m_pPixelShader = nullptr;	// �ȼ� ���̴�.	

	ID3D11VertexShader* m_pSkeletalMeshVertexShader = nullptr;	// SkeletalMesh ���� ���̴�.
	ID3D11InputLayout* m_pSkeletalMeshInputLayout = nullptr;	// SkeletalMesh �Է� ���̾ƿ�.

	ID3D11VertexShader* m_pStaticMeshVertexShader = nullptr;	// StaticMesh ���� ���̴�.
	ID3D11InputLayout* m_pStaticMeshInputLayout = nullptr;		// StaticMesh �Է� ���̾ƿ�.


	ID3D11Buffer* m_pConstantBuffer = nullptr;		// ��� ����.
	ID3D11Buffer* m_pBoolBuffer = nullptr;		    // ��� ����.
	ID3D11Buffer* m_pTransformBuffer = nullptr;		// ��� ����.
	ID3D11Buffer* m_pLightBuffer = nullptr;		    // ��� ����.
	ID3D11Buffer* m_pMatPalette = nullptr;		    // ��� ����.
	ID3D11Buffer* m_pMaterialBuffer = nullptr;		    // ��� ����.
	ConstantBuffer<CB_MatrixPalette> m_cbMatrixPallete; // DirectXTK�� ������� Ŭ���� Ȱ��

	std::list<StaticMeshInstance*> m_StaticMeshInstance;		//  �������� �𵨵��� ������ �����صд�. 
	std::list<SkeletalMeshInstance*> m_SkeletalMeshInstance;		//  �������� �𵨵��� ������ �����صд�. 

	Model* m_pModel = nullptr;

	// ���̴��� �� ������ ������
	Matrix   m_World;		// ������ǥ�� �������� ��ȯ�� ���� ���.
	Matrix   m_View;		// ī�޶���ǥ�� �������� ��ȯ�� ���� ���.
	Matrix   m_Projection;	// ������ġ��ǥ��( Normalized Device Coordinate) �������� ��ȯ�� ���� ���.

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

