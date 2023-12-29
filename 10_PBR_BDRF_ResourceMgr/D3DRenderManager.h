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
	static ID3D11Device* m_pDevice;					 // ����̽�
	ID3D11DeviceContext* m_pDeviceContext = nullptr; // ��� ����̽� ���ؽ�Ʈ




	//  Data  //
public: 
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
	ID3D11Buffer* m_pMatPalette = nullptr;		    // ��� ����.

	XMVECTOR m_Eye;
	XMVECTOR m_At;
	XMVECTOR m_Up;

	// ���̴��� �� ������ ������
	Matrix   m_World;		// ������ǥ�� �������� ��ȯ�� ���� ���.
	Matrix   m_View;		// ī�޶���ǥ�� �������� ��ȯ�� ���� ���.
	Matrix   m_Projection;	// ������ġ��ǥ��( Normalized Device Coordinate) �������� ��ȯ�� ���� ���.

	D3D11_VIEWPORT viewport = {};


	float m_Cam[3] = { 0.0f, -340.0f, 400.0f };


	//Node* m_pNode = nullptr;
	Model* m_pModel = nullptr;

	std::list<StaticMeshInstance*> m_StaticMeshInstance;		//  �������� �𵨵��� ������ �����صд�. 
	std::list<StaticMeshComponent*> m_StaticMeshComponents;		//  �������� �𵨵��� ������ �����صд�.

};