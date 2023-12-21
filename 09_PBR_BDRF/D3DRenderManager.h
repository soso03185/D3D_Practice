#pragma once
#include "TutorialApp.h"

#include <dxgi1_4.h>

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

class D3DRenderManager
{
	D3DRenderManager();
	~D3DRenderManager();

	static D3DRenderManager* Instance;
	ComPtr<IDXGIFactory4> m_pDXGIFactory;		// DXGI���丮
	ComPtr<IDXGIAdapter3> m_pDXGIAdapter;		// ����ī�� ������ ���� ������ �������̽�


	static ID3D11Device* m_pDevice;							// ����̽�	
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
	ID3D11Buffer* m_pMatPalette = nullptr;		    // ��� ����.

	//Node* m_pNode = nullptr;
	Model* m_pModel = nullptr;

};

