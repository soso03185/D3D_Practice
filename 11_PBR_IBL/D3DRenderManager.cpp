
#include "..\Common\Helper.h"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "D3DRenderManager.h"
#include "Material.h"
#include "ModelResource.h"
#include "EnvironmentMeshComponent.h"

#include <d3d11.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <Psapi.h>


D3DRenderManager* D3DRenderManager::Instance = nullptr;
ID3D11Device* D3DRenderManager::m_pDevice = nullptr;

// ModelResource -> ������ �������� ������ ���
// MeshInstance  -> ������ �ϱ� ���� �� �޽��� ������
// MeshComponent -> �����͸� ��� �ִ� ������

D3DRenderManager::D3DRenderManager()
{
	assert(Instance == nullptr);
	Instance = this;
}

D3DRenderManager::~D3DRenderManager()
{
	Uninitialize();
}

void D3DRenderManager::GetVideoMemoryInfo(std::string& out)
{
	DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo;
	m_pDXGIAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);

	out = std::to_string(videoMemoryInfo.CurrentUsage / 1024 / 1024) + " MB" + " / " + std::to_string(videoMemoryInfo.Budget / 1024 / 1024) + " MB";
}

void D3DRenderManager::GetSystemMemoryInfo(std::string& out)
{
	HANDLE hProcess = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS_EX);
	GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	out = std::to_string((pmc.PagefileUsage) / 1024 / 1024) + " MB";
}

void D3DRenderManager::ApplyMaterial(Material* pMaterial)
{
	ID3D11ShaderResourceView* pNullSRV[7] = {
		pMaterial->m_pDiffuseRV != nullptr ? pMaterial->m_pDiffuseRV->m_pTextureSRV.Get() : nullptr,
		pMaterial->m_pNormalRV != nullptr ? pMaterial->m_pNormalRV->m_pTextureSRV.Get() : nullptr,
		pMaterial->m_pSpecularRV != nullptr ? pMaterial->m_pSpecularRV->m_pTextureSRV.Get() : nullptr,
		pMaterial->m_pEmissiveRV != nullptr ? pMaterial->m_pEmissiveRV->m_pTextureSRV.Get() : nullptr,
		pMaterial->m_pOpacityRV != nullptr ? pMaterial->m_pOpacityRV->m_pTextureSRV.Get() : nullptr,
		pMaterial->m_pMetalnessRV != nullptr ? pMaterial->m_pMetalnessRV->m_pTextureSRV.Get() : nullptr,
		pMaterial->m_pRoughnessRV != nullptr ? pMaterial->m_pRoughnessRV->m_pTextureSRV.Get() : nullptr ,
	};

	m_pDeviceContext->PSSetShaderResources(0, 7, pNullSRV); // �ѹ��� 7���� �ؽ�ó�� �����Ѵ�.

	CB_BoolBuffer CB_Bool;
	CB_Bool.UseGamma = isGamma;
	CB_Bool.UseDiffuseMap = pMaterial->m_pDiffuseRV != nullptr ? true : false;
	CB_Bool.UseNormalMap = pMaterial->m_pNormalRV != nullptr ? true : false;
	CB_Bool.UseSpecularMap = pMaterial->m_pSpecularRV != nullptr ? true : false;
	CB_Bool.UseEmissiveMap = pMaterial->m_pEmissiveRV != nullptr ? true : false;
	CB_Bool.UseOpacityMap = pMaterial->m_pOpacityRV != nullptr ? true : false;
	CB_Bool.UseMetalnessMap = pMaterial->m_pMetalnessRV != nullptr ? true : false;
	CB_Bool.UseRoughnessMap = pMaterial->m_pRoughnessRV != nullptr ? true : false;

	if (CB_Bool.UseOpacityMap && m_pAlphaBlendState != nullptr)
		m_pDeviceContext->OMSetBlendState(m_pAlphaBlendState, nullptr, 0xffffffff); // ���ĺ��� ���¼��� , �ٸ��ɼ��� �⺻�� 
	else
		m_pDeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);	// �������� , �ٸ��ɼ��� �⺻��

	m_pDeviceContext->UpdateSubresource(m_pBoolBuffer, 0, nullptr, &CB_Bool, 0, 0);
}

void D3DRenderManager::AddMeshInstance(StaticMeshComponent* pModel)
{
	for (size_t i = 0; i < pModel->m_MeshInstances.size(); i++)
	{
		m_StaticMeshInstance.push_back(&pModel->m_MeshInstances[i]);
	}
}

void D3DRenderManager::AddMeshInstance(SkeletalMeshComponent* pModel)
{
	for (size_t i = 0; i < pModel->m_MeshInstances.size(); i++)
	{
		m_SkeletalMeshInstance.push_back(&pModel->m_MeshInstances[i]);
	}
}

void D3DRenderManager::ConstantBuffUpdate()
{
	///  ConstantBuffer Binding  /// 
	CB_BoolBuffer CB_Bool;
	CB_Bool.UseGamma = isGamma;

	CB_LightDirBuffer CB_Light;
	CB_Light.vLightColor.x = m_vLightColor[0];
	CB_Light.vLightColor.y = m_vLightColor[1];
	CB_Light.vLightColor.z = m_vLightColor[2];
	CB_Light.vLightColor.w = 1.0f;

	CB_Light.vLightDir.x = m_vLightDir[0];
	CB_Light.vLightDir.y = m_vLightDir[1];
	CB_Light.vLightDir.z = m_vLightDir[2];
	CB_Light.vLightDir.w = 1.0f;

	CB_Light.vLightDir.Normalize();
	CB_Light.mWorldCameraPosition = XMVectorSet(m_Cam[0], m_Cam[1], m_Cam[2], 0.0f);

	m_TransformVP.mView = XMMatrixTranspose(m_View);
	m_TransformVP.mProjection = XMMatrixTranspose(m_Projection);

	m_pDeviceContext->UpdateSubresource(m_pTransformVP_Buffer, 0, nullptr, &m_TransformVP, 0, 0);
	m_pDeviceContext->UpdateSubresource(m_pIBL_Buffer, 0, nullptr, &m_IBL, 0, 0);
	m_pDeviceContext->UpdateSubresource(m_pLightBuffer, 0, nullptr, &CB_Light, 0, 0);
}

bool D3DRenderManager::Initialize(UINT Width, UINT Height, HWND hWnd)
{
	m_ClientWidth = Width;
	m_ClientHeight = Height;
	m_hWnd = hWnd;

	if (!InitD3D())		return false;
	if (!InitImGUI())	return false;


	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_previousTime);
	QueryPerformanceCounter(&m_currentTime);

	return true;
}

void D3DRenderManager::IncreaseStaticModel(std::string pilePath)
{
	// 8. FBX Load	
	StaticMeshComponent* newModel = new StaticMeshComponent();
	newModel->ReadSceneResourceFromFBX(pilePath);
 	int range = 500;
	float posx = (float)(rand() % range) - range * 0.5f;
	float posy = (float)(rand() % range) - range * 0.5f;
	float posz = (float)(rand() % range) - range * 0.5f;
	newModel->SetLocalPosition(Math::Vector3(posx, posy, posz));
}

void D3DRenderManager::IncreaseSkeletalModel(std::string pilePath)
{
	// 8. FBX Load	
	SkeletalMeshComponent* newModel = new SkeletalMeshComponent();
	newModel->ReadSceneResourceFromFBX(pilePath);
	newModel->AddSceneAnimationFromFBX("../Resource/Zombie_Run.fbx");

	int range = 500;
	float posx = (float)(rand() % range) - range * 0.5f;
	float posy = (float)(rand() % range) - range * 0.5f;
	float posz = (float)(rand() % range) - range * 0.5f;
	newModel->SetLocalPosition(Math::Vector3(posx, posy, posz));
	newModel->PlayAnimation(0);
}

void D3DRenderManager::DecreaseModel()
{
	if (D3DRenderManager::Instance->m_SkeletalMeshComponents.size() > 0)
		D3DRenderManager::Instance->m_SkeletalMeshComponents.pop_back();

	if (D3DRenderManager::Instance->m_StaticMeshComponents.size() > 0)
		D3DRenderManager::Instance->m_StaticMeshComponents.pop_back();
}

bool D3DRenderManager::InitD3D()
{
	// �����.
	HRESULT hr = 0;

	// Create DXGI factory
	HR_T(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)m_pDXGIFactory.GetAddressOf()));
	HR_T(m_pDXGIFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter**>(m_pDXGIAdapter.GetAddressOf())));

	// ����� �ػ󵵸� ���´ٰ� �����մϴ�.
	int monitorWidth = (float)m_ClientWidth;  // ������� ���� �ػ�
	int monitorHeight = (float)m_ClientHeight;  // ������� ���� �ػ�

	viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = monitorWidth;
	viewport.Height = monitorHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;


	// ����ü�� �Ӽ� ���� ����ü ����.
	CreateSwapChain();

	// ����Ÿ�ٺ� ����.  (����۸� �̿��ϴ� ����Ÿ�ٺ�)	
	CreateRenderTargetView();

	// �����Ͷ����� ����
	CreateRasterizerState();

	//? DepthStencil ���� ����
	CreateDepthStencilState();

	// depth & stencil view ����
	CreateStencilAndDepth();

	// ���ĺ����� ���� ���� ���� ����
	CreateBlendState();

	// Render() ���� ���������ο� ���ε��� ��� ���� ����  //  Create the constant buffer 
	CreateConstantBuffer();

	// Render() ���� ���������ο� ���ε��� ���̴� ���ҽ��� ���÷� ���� (�ؽ�ó �ε� & sample state ���� )
	CreateSamplerState();

	// Render() ���� ���������ο� ���ε��� ���ؽ� ���̴� ����
	CreateVS_IL();

	// Render() ���� ���������ο� ���ε��� �ȼ� ���̴� ����
	CreatePS();

	// Render() ���� ���������ο� ���ε��� ȯ�� �ȼ� ���̴� ����
	CreateEnvironment();

	// ȯ�� �� ����
	CreateIBL();

	// ������ �ʱ�ȭ
	InitScene();

	return true;
}

void D3DRenderManager::InitScene()
{
	// ���̴��� ������ ������ ����
	m_Eye = XMVectorSet(m_Cam[0], m_Cam[1], m_Cam[2], 0.0f);
	m_At = XMVectorSet(m_Cam[0], m_Cam[1], 0.0f, 0.0f);
	m_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_View = XMMatrixLookAtLH(m_Eye, m_At, m_Up);

	// Initialize the projection matrix  	// fov
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.1f, 1000.0f);


}

bool D3DRenderManager::InitImGUI()
{
	// ImGui �ʱ�ȭ.

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(this->m_pDevice, this->m_pDeviceContext);

	return true;
}

void D3DRenderManager::Uninitialize()
{
	SAFE_RELEASE(m_pStaticVertexShader);
	SAFE_RELEASE(m_pSkeletalVertexShader);
	SAFE_RELEASE(m_pStaticInputLayout);
	SAFE_RELEASE(m_pSkeletalInputLayout);
	SAFE_RELEASE(m_pPixelShader);

	SAFE_RELEASE(m_pSamplerLinear);
	SAFE_RELEASE(m_pSamplerClamp);

	SAFE_RELEASE(m_pEnvironmentVertexShader);
	SAFE_RELEASE(m_pEnvironmentPixelShader);

	SAFE_RELEASE(m_pIBL_Buffer);
	SAFE_RELEASE(m_pBoolBuffer);
	SAFE_RELEASE(m_pTransformW_Buffer);
	SAFE_RELEASE(m_pTransformVP_Buffer);
	SAFE_RELEASE(m_pLightBuffer);
	SAFE_RELEASE(m_pAlphaBlendState);
	SAFE_RELEASE(m_pMatPalette);

	// Cleanup DirectX
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pRenderTargetView);

	// Cleanup Imgui
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void D3DRenderManager::CreateVS_IL()
{
	HRESULT hr = 0; // �����.
	{
		D3D_SHADER_MACRO defines[] =
		{
			{"VERTEX_SKINNING",""}, // ��ũ�� �̸��� ���� ����
			{nullptr, nullptr}    // �迭�� ���� ��Ÿ���� ���� nullptr�� ������.
		};

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 4. Render() ���� ���������ο� ���ε��� �ε��� ���� ����
		ID3D10Blob* vertexShaderBuffer = nullptr;	// ���� ���̴� �ڵ尡 ����� ����.
		HR_T(CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_5_0", &vertexShaderBuffer, defines));
		HR_T(m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
			vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pSkeletalInputLayout));

		// 3. Render() ���� ���������ο� ���ε���  ���ؽ� ���̴� ����
		HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(), NULL, &m_pSkeletalVertexShader));
		SAFE_RELEASE(vertexShaderBuffer);
	}

	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ID3D10Blob* vertexShaderBuffer = nullptr;	// ���� ���̴� �ڵ尡 ����� ����.
		HR_T(CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_5_0", &vertexShaderBuffer, nullptr));
		HR_T(m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
			vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pStaticInputLayout));

		// 3. Render() ���� ���������ο� ���ε���  ���ؽ� ���̴� ����
		HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(), NULL, &m_pStaticVertexShader));
		SAFE_RELEASE(vertexShaderBuffer);
	}
}

void D3DRenderManager::CreatePS()
{
	ID3D10Blob* pixelShaderBuffer = nullptr;	// �ȼ� ���̴� �ڵ尡 ����� ����.
	//HR_T(CompileShaderFromFile(L"BasicPixelShader.hlsl", "main", "ps_5_0", &pixelShaderBuffer, nullptr));
	HR_T(CompileShaderFromFile(L"PBR_PixelShader.hlsl", "main", "ps_5_0", &pixelShaderBuffer, nullptr));
	HR_T(m_pDevice->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));
	SAFE_RELEASE(pixelShaderBuffer);
}

void D3DRenderManager::CreateEnvironment()
{
	// 2. Render() ���� ���������ο� ���ε��� InputLayout ���� 	
	D3D_SHADER_MACRO defines[] =
	{
		{"",""}, // ��ũ�� �̸��� ���� ����
		{nullptr, nullptr}    // �迭�� ���� ��Ÿ���� ���� nullptr�� ������.
	};

	ComPtr<ID3D10Blob> buffer = nullptr;
	HR_T(CompileShaderFromFile(L"VS_Environment.hlsl", "main", "vs_5_0", buffer.GetAddressOf(), nullptr));
	HR_T(m_pDevice->CreateVertexShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, &m_pEnvironmentVertexShader));
	buffer.Reset();

	HR_T(CompileShaderFromFile(L"PS_Environment.hlsl", "main", "ps_5_0", buffer.GetAddressOf(), nullptr));
	HR_T(m_pDevice->CreatePixelShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, &m_pEnvironmentPixelShader));
}

void D3DRenderManager::CreateSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR_T(m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear));

	sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.MaxAnisotropy = (sampDesc.Filter == D3D11_FILTER_ANISOTROPIC) ? D3D11_REQ_MAXANISOTROPY : 1;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR_T(m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerClamp));

	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	m_pDeviceContext->PSSetSamplers(1, 1, &m_pSamplerClamp);
}

void D3DRenderManager::CreateBlendState()
{
	D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
	rtBlendDesc.BlendEnable = true;						// ���� ��� ����
	// FinalRGB = (SrcRGB * SrcBlend) + (DestRGB * DestBlend)
	rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;	    // SrcBlend�� SrcColor�� ���İ�
	rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	// DestBlend�� (1-SourceColor.a)
	rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;			// �÷� ���� ������ �����մϴ�.
	// FinalAlpha = (SrcAlpha * SrcBlendAlpha) + (DestAlpha * DestBlendAlpha)
	rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;		// ���� ���� ������ �����մϴ�.
	rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;		// SrcBlendAlpha = 1
	rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;		// DestBlendAlpha = 1	
	rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // ����Ÿ�ٿ� RGBA ��� Write	// ���� ���� Ÿ�ٿ� ���� �÷� ������Ʈ ���� ����ũ�� �����մϴ�.

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;  // ���� ���ø� ��ƿ�����(Anti-aliasing)�� �����ϱ� ���� ���
	blendDesc.IndependentBlendEnable = false; // �� ���� Ÿ�ٿ� ���� �������� ���� ������ ������� ����
	blendDesc.RenderTarget[0] = rtBlendDesc;
	HR_T(m_pDevice->CreateBlendState(&blendDesc, &m_pAlphaBlendState));
}

void D3DRenderManager::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	bd.ByteWidth = sizeof(CB_IBL);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pIBL_Buffer));

	bd.ByteWidth = sizeof(CB_BoolBuffer);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pBoolBuffer));

	bd.ByteWidth = sizeof(CB_TransformW);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pTransformW_Buffer));

	bd.ByteWidth = sizeof(CB_TransformVP);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pTransformVP_Buffer));

	bd.ByteWidth = sizeof(CB_LightDirBuffer);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pLightBuffer));

	bd.ByteWidth = sizeof(CB_MatrixPalette);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pMatPalette));

}

void D3DRenderManager::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;	// ����ü�� ����� â �ڵ� ��.
	swapDesc.Windowed = true;		// â ��� ���� ����.
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �����(�ؽ�ó)�� ����/���� ũ�� ����.
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Height = m_ClientHeight;
	// ȭ�� �ֻ��� ����.
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// ���ø� ���� ����.
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	UINT creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 1. ��ġ ����.   2.����ü�� ����. 3.��ġ ���ؽ�Ʈ ����.
	HR_T(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL,
		D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext));
}

void D3DRenderManager::CreateStencilAndDepth()
{
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = m_ClientWidth;
	descDepth.Height = m_ClientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Texture2D* textureDepthStencil = nullptr;
	HR_T(m_pDevice->CreateTexture2D(&descDepth, nullptr, &textureDepthStencil));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	HR_T(m_pDevice->CreateDepthStencilView(textureDepthStencil, &descDSV, &m_pDepthStencilView));
	SAFE_RELEASE(textureDepthStencil);

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

}

void D3DRenderManager::CreateRenderTargetView()
{
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	HR_T(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView));  // �ؽ�ó�� ���� ���� ����

	SAFE_RELEASE(pBackBufferTexture);	//�ܺ� ���� ī��Ʈ�� ���ҽ�Ų��.
}

void D3DRenderManager::CreateRasterizerState()
{
	HRESULT hr = 0; 

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.AntialiasedLineEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthClipEnable = true;
	HR_T(m_pDevice->CreateRasterizerState(&rasterizerDesc, m_pRasterizerStateCCW.GetAddressOf()));

	rasterizerDesc.FrontCounterClockwise = false;
	HR_T(m_pDevice->CreateRasterizerState(&rasterizerDesc, m_pRasterizerStateCW.GetAddressOf()));
}

void D3DRenderManager::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC lessEqualDesc;
	lessEqualDesc.DepthEnable = true;
	lessEqualDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	lessEqualDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // ���� ������ �� ���� �ѵ� ���
	lessEqualDesc.StencilEnable = false;

	HR_T(m_pDevice->CreateDepthStencilState(&lessEqualDesc, &m_LessEqualDSS));
}

void D3DRenderManager::CreateIBL()
{
	EnvironmentMeshComponent* pComponent = new EnvironmentMeshComponent();
	pComponent->ReadEnvironmentMeshFromFBX("../Resource/EnvironmentCube.fbx");
	pComponent->ReadEnvironmentTextureFromDDS(L"../Resource/BakerSampleEnvHDR.dds");
	pComponent->ReadIBLDiffuseTextureFromDDS(L"../Resource/BakerSampleDiffuseHDR.dds");
	pComponent->ReadIBLSpecularTextureFromDDS(L"../Resource/BakerSampleSpecularHDR.dds");
	pComponent->ReadIBLBRDFTextureFromDDS(L"../Resource/BakerSampleBRDF.dds");
	
	pComponent->SetLocalPosition(Vector3(100, 100, 100));
	pComponent->SetLocalScale(Vector3(1000, 1000, 1000));

	SetEnvironment(pComponent);
}

void D3DRenderManager::SetEnvironment(EnvironmentMeshComponent* val)
{	
	m_pEnvironmentMeshComponent = val;
	auto component = m_pEnvironmentMeshComponent;	// Shared.hlsli ���� �ؽ�ó slot7 Ȯ��

	// Shared.hlsli ���� �ؽ�ó slot7 Ȯ��
	m_pDeviceContext->PSSetShaderResources(7, 1, component->m_EnvironmentTextureResource->m_pTextureSRV.GetAddressOf());
	m_pDeviceContext->PSSetShaderResources(8, 1, component->m_IBLDiffuseTextureResource->m_pTextureSRV.GetAddressOf());
	m_pDeviceContext->PSSetShaderResources(9, 1, component->m_IBLSpecularTextureResource->m_pTextureSRV.GetAddressOf());
	m_pDeviceContext->PSSetShaderResources(10, 1, component->m_IBLBRDFTextureResource->m_pTextureSRV.GetAddressOf());
	m_IBL.UseIBL = true;
	m_pDeviceContext->UpdateSubresource(m_pIBL_Buffer, 0, nullptr, &m_IBL, 0, 0);
}

void D3DRenderManager::Update()
{
	// Cam Transform
	m_Eye = XMVectorSet(m_Cam[0], m_Cam[1], m_Cam[2], 0.0f);
	// m_At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_At = XMVectorSet(m_Look[0], m_Look[1], m_Look[2], 0.0f);
	m_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookToLH(m_Eye, m_At, m_Up);

	m_Projection = XMMatrixPerspectiveFovLH(m_Fov / 180.0f * 3.14f, m_ClientWidth / (FLOAT)m_ClientHeight, m_Near, m_Far);

	m_previousTime = m_currentTime;
	QueryPerformanceCounter(&m_currentTime);
	m_deltaTime = static_cast<float>(m_currentTime.QuadPart - m_previousTime.QuadPart) / static_cast<float>(m_frequency.QuadPart);

#ifdef _DEBUG
	if (m_deltaTime > (1.0f / 60.0f))
		m_deltaTime = (1.0f / 60.0f);
#endif	

	ConstantBuffUpdate();

	for (auto& StaticMeshComponent : m_StaticMeshComponents)
	{
		// �ϳ��� �޽� ������Ʈ�� �������� �޽� Instance �� ����������.
		AddMeshInstance(StaticMeshComponent);
		StaticMeshComponent->Update(m_deltaTime);
	}

	for (auto& SkeletalMeshComponent : m_SkeletalMeshComponents)
	{
		// �ϳ��� �޽� ������Ʈ�� �������� �޽� Instance �� ����������.
		AddMeshInstance(SkeletalMeshComponent);
		SkeletalMeshComponent->Update(m_deltaTime);
	}

	if (m_pEnvironmentMeshComponent != nullptr)
	{
		m_pEnvironmentMeshComponent->Update(m_deltaTime);
		isIBL ? m_IBL.UseIBL = true : m_IBL.UseIBL = false;
	}
}

void D3DRenderManager::Render()
{
	// Draw�迭 �Լ��� ȣ���ϱ����� ������ ���������ο� �ʼ� �������� ������ �ؾ��Ѵ�	
	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// vertex shader
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pIBL_Buffer);
	m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pBoolBuffer);
	m_pDeviceContext->VSSetConstantBuffers(2, 1, &m_pTransformW_Buffer);
	m_pDeviceContext->VSSetConstantBuffers(3, 1, &m_pTransformVP_Buffer);
	m_pDeviceContext->VSSetConstantBuffers(4, 1, &m_pLightBuffer);
	m_pDeviceContext->VSSetConstantBuffers(5, 1, &m_pMatPalette);

	// pixel shader
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pIBL_Buffer);
	m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pBoolBuffer);
	m_pDeviceContext->PSSetConstantBuffers(2, 1, &m_pTransformW_Buffer);
	m_pDeviceContext->PSSetConstantBuffers(3, 1, &m_pTransformVP_Buffer);
	m_pDeviceContext->PSSetConstantBuffers(4, 1, &m_pLightBuffer);

	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	m_pDeviceContext->RSSetViewports(1, &viewport);

	if (m_pEnvironmentMeshComponent != nullptr)
		RenderEnvironment();

	RenderStaticMeshInstance();
	RenderSkeletalMeshInstance();

	ImguiRender();
	m_pSwapChain->Present(0, 0);
}

void D3DRenderManager::ImguiRender()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Editor");

		std::string str;
		GetVideoMemoryInfo(str);
		ImGui::Text("VideoMemory: %s", str.c_str());
		GetSystemMemoryInfo(str);
		ImGui::Text("SystemMemory: %s", str.c_str());

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat3("Cam_Pos", m_Cam, -500.0f, 500.0f);
		ImGui::SliderFloat3("Look_Pos", m_Look, -180, 180);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Checkbox("IBL", &isIBL);
		ImGui::Checkbox("NormalMap", &isNormalMap);
		ImGui::Checkbox("SpecularMap", &isSpecularMap);
		ImGui::Checkbox("Gamma_Correction", &isGamma);
		ImGui::Checkbox("DiffuseMap", &isDiffuse);
		ImGui::Checkbox("EmissiveMap", &isEmissive);
		ImGui::Checkbox("OpacityMap", &isOpacity);
		ImGui::Checkbox("MetalnessMap", &isMetalness);
		ImGui::Checkbox("RoughnessMap", &isRoughness);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat("Far", &m_Far, 1.0f, 10000.0f);
		ImGui::SliderFloat("Near", &m_Near, 0.01f, 1000.0f);
		ImGui::SliderFloat("Fov", &m_Fov, -20.0f, 180.0f);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat3("Light_RGB", m_vLightColor, 0.0f, 1.0f);
		ImGui::SliderFloat3("Light_XYZ", m_vLightDir, -1.0f, 1.0f);

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

void D3DRenderManager::RenderStaticMeshInstance()
{
	m_pDeviceContext->IASetInputLayout(m_pStaticInputLayout);
	m_pDeviceContext->VSSetShader(m_pStaticVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->RSSetState(m_pRasterizerStateCW.Get());

	m_StaticMeshInstance.sort([](const StaticMeshInstance* lhs, const StaticMeshInstance* rhs)
		{
			return lhs->m_pMaterial < rhs->m_pMaterial;
		});

	Material* pPrevMaterial = nullptr;

	for (const auto& meshInstance : m_StaticMeshInstance)
	{
		if (pPrevMaterial != meshInstance->m_pMaterial)
		{
			ApplyMaterial(meshInstance->m_pMaterial);	// ���͸��� ����
			pPrevMaterial = meshInstance->m_pMaterial;
		}

		//? Static Mesh
		m_TransformW.mWorld = meshInstance->m_pNodeWorldTransform->Transpose();
		m_pDeviceContext->UpdateSubresource(m_pTransformW_Buffer, 0, nullptr, &m_TransformW, 0, 0);

		// Draw
		meshInstance->Render(m_pDeviceContext);
	}
	m_StaticMeshInstance.clear();
}

void D3DRenderManager::RenderSkeletalMeshInstance()
{
	m_pDeviceContext->IASetInputLayout(m_pSkeletalInputLayout);
	m_pDeviceContext->VSSetShader(m_pSkeletalVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->RSSetState(m_pRasterizerStateCW.Get());

	m_SkeletalMeshInstance.sort([](const SkeletalMeshInstance* lhs, const SkeletalMeshInstance* rhs)
		{
			return lhs->m_pMaterial < rhs->m_pMaterial;
		});

	Material* pPrevMaterial = nullptr;
	for (const auto& meshInstance : m_SkeletalMeshInstance)
	{
		if (pPrevMaterial != meshInstance->m_pMaterial)
		{
			ApplyMaterial(meshInstance->m_pMaterial);	// ���͸��� ����
			pPrevMaterial = meshInstance->m_pMaterial;
		}

		//? Skeletal Mesh
		CB_MatrixPalette CB_MatPalatte;
		meshInstance->UpdateMatrixPallete(CB_MatPalatte.Array);
		m_pDeviceContext->UpdateSubresource(m_pMatPalette, 0, nullptr, &CB_MatPalatte, 0, 0);

		// Draw
		meshInstance->Render(m_pDeviceContext);
	}
	m_SkeletalMeshInstance.clear();
}

void D3DRenderManager::RenderEnvironment()
{
	// ���� ���Ľ� ���´� �̰͸� ��������.
	m_pDeviceContext->OMSetDepthStencilState(m_LessEqualDSS.Get(), 0);

	m_pDeviceContext->IASetInputLayout(m_pStaticInputLayout);
	m_pDeviceContext->VSSetShader(m_pEnvironmentVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pEnvironmentPixelShader, nullptr, 0);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pTransformW_Buffer); //debugdraw���� ������Ѽ� �����Ѵ�.
	m_pDeviceContext->RSSetState(m_pRasterizerStateCCW.Get());

	m_TransformW.mWorld = m_pEnvironmentMeshComponent->m_World.Transpose();
	m_pDeviceContext->UpdateSubresource(m_pTransformW_Buffer, 0, nullptr, &m_TransformW, 0, 0);
	m_pEnvironmentMeshComponent->m_MeshInstance.Render(m_pDeviceContext);
 }
