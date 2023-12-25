
#include "D3DRenderManager.h"
#include "../Common/Helper.h"

#include "StaticMeshInstance.h"
#include "StaticMeshResource.h"
#include "StaticMeshComponent.h"

#include "SkeletalMeshInstance.h"
#include "SkeletalMeshResource.h"
#include "SkeletalMeshComponent.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <d3dcompiler.h>
#include <Psapi.h>

D3DRenderManager* D3DRenderManager::Instance = nullptr;

ID3D11Device* D3DRenderManager::m_pDevice = nullptr;

D3DRenderManager::D3DRenderManager()
{
	assert(Instance == nullptr);
	Instance = this;
}

D3DRenderManager::~D3DRenderManager()
{
	Uninitialize();
}



void D3DRenderManager::AddMeshInstance(SkeletalMeshModel* pModel)
{
	for (size_t i = 0; i < pModel->m_MeshInstances.size(); i++)
	{
		m_SkeletalMeshInstance.push_back(&pModel->m_MeshInstances[i]);
	}
}

void D3DRenderManager::AddMeshInstance(StaticMeshModel* pModel)
{
	for (size_t i = 0; i < pModel->m_MeshInstances.size(); i++)
	{
		m_StaticMeshInstance.push_back(&pModel->m_MeshInstances[i]);
	}
}


bool D3DRenderManager::Initialize(HWND Handle, UINT Width, UINT Height)
{
	m_hWnd = Handle;
	HRESULT hr = 0;	// �����.

	// Create DXGI factory
	HR_T(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)m_pDXGIFactory.GetAddressOf()));
	HR_T(m_pDXGIFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter**>(m_pDXGIAdapter.GetAddressOf())));


	// ����ü�� �Ӽ� ���� ����ü ����.
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

	// 4. ����Ÿ�ٺ� ����.  (����۸� �̿��ϴ� ����Ÿ�ٺ�)	
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	HR_T(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView));  // �ؽ�ó�� ���� ���� ����

	SAFE_RELEASE(pBackBufferTexture);	//�ܺ� ���� ī��Ʈ�� ���ҽ�Ų��.

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
	m_pDeviceContext->RSSetViewports(1, &viewport);

	// 6. depth & stencil view ����
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


	// 7. ���ĺ����� ���� ���� ���� ����
	// FinalAlpha = (SrcAlpha * SrcBlendAlpha) + (DestAlpha * DestBlendAlpha)
	// FinalRGB = (SrcRGB * SrcBlend) + (DestRGB * DestBlend)

	D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
	rtBlendDesc.BlendEnable = true;						// ���� ��� ����
	rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;	    // SrcBlend�� SrcColor�� ���İ�
	rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	// DestBlend�� (1-SourceColor.a)
	rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;			// �÷� ���� ������ �����մϴ�.
	rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;		// ���� ���� ������ �����մϴ�.
	rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;		// SrcBlendAlpha = 1
	rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;		// DestBlendAlpha = 1	
	rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // ����Ÿ�ٿ� RGBA ��� Write	// ���� ���� Ÿ�ٿ� ���� �÷� ������Ʈ ���� ����ũ�� �����մϴ�.

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;  // ���� ���ø� ��ƿ�����(Anti-aliasing)�� �����ϱ� ���� ���
	blendDesc.IndependentBlendEnable = false; // �� ���� Ÿ�ٿ� ���� �������� ���� ������ ������� ����
	blendDesc.RenderTarget[0] = rtBlendDesc;
	HR_T(m_pDevice->CreateBlendState(&blendDesc, &m_pAlphaBlendState));


	/*
		ImGui �ʱ�ȭ.
	*/

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(this->m_pDevice, this->m_pDeviceContext);

	// * Render() ���� ���������ο� ���ε��� ���ؽ� ���̴� ����
	CreateSkeletalMesh_VS_IL();
	CreateStaticMesh_VS_IL();


	// * Render() ���� ���������ο� ���ε��� �ȼ� ���̴� ����
	CreatePS();


	// 6. Render() ���� ���������ο� ���ε��� ��� ���� ����
	// Create the constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	bd.ByteWidth = sizeof(CB_ConstantBuffer);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer));

	bd.ByteWidth = sizeof(CB_BoolBuffer);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pBoolBuffer));

	bd.ByteWidth = sizeof(CB_TransformBuffer);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pTransformBuffer));

	bd.ByteWidth = sizeof(CB_LightDirBuffer);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pLightBuffer));

	bd.ByteWidth = sizeof(CB_MatrixPalette);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pMatPalette));
	
	bd.ByteWidth = sizeof(CB_MatrixPalette);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pMaterialBuffer));
	
	m_cbMatrixPallete.Create(m_pDevice);

	// 7. Render() ���� ���������ο� ���ε��� ���̴� ���ҽ��� ���÷� ���� (�ؽ�ó �ε� & sample state ���� ) 
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR_T(m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear));

	// ���̴��� ������ ������ ����
	// Initialize the world matrix
	m_World = XMMatrixIdentity();
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -1000.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_View = XMMatrixLookAtLH(Eye, At, Up);

	// Initialize the projection matrix  	// fov
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);

	return true;
}

void D3DRenderManager::Uninitialize()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// Cleanup DirectX
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pRenderTargetView);


	SAFE_RELEASE(m_pSkeletalMeshVertexShader);
	SAFE_RELEASE(m_pSkeletalMeshInputLayout);
	SAFE_RELEASE(m_pStaticMeshVertexShader);
	SAFE_RELEASE(m_pStaticMeshInputLayout);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pBoolBuffer);
	SAFE_RELEASE(m_pTransformBuffer);
	SAFE_RELEASE(m_pLightBuffer);
	SAFE_RELEASE(m_pAlphaBlendState);
	SAFE_RELEASE(m_pMatPalette);
}

void D3DRenderManager::Update()
{
	m_World = Matrix::CreateScale(m_Scale) *
			  Matrix::CreateFromYawPitchRoll(Vector3(
					XMConvertToRadians(m_Rotation.x),
					XMConvertToRadians(m_Rotation.y),
					XMConvertToRadians(m_Rotation.z)));

	// Cam Transform
	m_View = XMMatrixLookToLH(m_CameraPos, Vector3(0, 0, 1), Vector3(0, 1, 0));

	CB_Light.EyePosition = m_CameraPos;
}

void D3DRenderManager::Render()
{
	// Draw�迭 �Լ��� ȣ���ϱ����� ������ ���������ο� �ʼ� �������� ������ �ؾ��Ѵ�	
	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// vertex shader
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pBoolBuffer);
	m_pDeviceContext->VSSetConstantBuffers(2, 1, &m_pTransformBuffer);
	m_pDeviceContext->VSSetConstantBuffers(3, 1, &m_pLightBuffer);
	m_pDeviceContext->VSSetConstantBuffers(4, 1, &m_pMatPalette);

	// 3 material
	auto buffer = m_cbMatrixPallete.GetBuffer();
	m_pDeviceContext->VSSetConstantBuffers(4, 1, &buffer);

	// pixel shader
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pBoolBuffer);
	m_pDeviceContext->PSSetConstantBuffers(2, 1, &m_pTransformBuffer);
	m_pDeviceContext->PSSetConstantBuffers(3, 1, &m_pLightBuffer);
	m_pDeviceContext->PSSetConstantBuffers(4, 1, &m_pMatPalette);

	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);

	CB_Light.vLightDir.Normalize();
	m_pDeviceContext->UpdateSubresource(m_pLightBuffer, 0, nullptr, &CB_Light, 0, 0);
	
	CB_Transform.mView = m_View.Transpose();
	CB_Transform.mProjection = m_Projection.Transpose();
	m_pDeviceContext->UpdateSubresource(m_pTransformBuffer, 0, nullptr, &CB_Transform, 0, 0);


	m_pDeviceContext->IASetInputLayout(m_pSkeletalMeshInputLayout);
	m_pDeviceContext->VSSetShader(m_pSkeletalMeshVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);


	// m_SkeletalMeshInstance
	m_pDeviceContext->IASetInputLayout(m_pSkeletalMeshInputLayout);
	m_pDeviceContext->VSSetShader(m_pSkeletalMeshVertexShader, nullptr, 0);


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
		// ����ȷ�Ʈ ������Ʈ						
		meshInstance->UpdateMatrixPallete(&CB_MatPalette);
		m_cbMatrixPallete.SetData(m_pDeviceContext, CB_MatPalette);

		// Draw
		meshInstance->Render(m_pDeviceContext);
	}
	m_SkeletalMeshInstance.clear();


	// m_StaticMeshInstance
	m_pDeviceContext->IASetInputLayout(m_pStaticMeshInputLayout);
	m_pDeviceContext->VSSetShader(m_pStaticMeshVertexShader, nullptr, 0);

	m_StaticMeshInstance.sort([](const StaticMeshInstance* lhs, const StaticMeshInstance* rhs)
		{
			return lhs->m_pMaterial < rhs->m_pMaterial;
		});
	for (const auto& meshInstance : m_StaticMeshInstance)
	{
		if (pPrevMaterial != meshInstance->m_pMaterial)
		{
			ApplyMaterial(meshInstance->m_pMaterial);	// ���͸��� ����
			pPrevMaterial = meshInstance->m_pMaterial;
		}

		CB_Transform.mWorld = meshInstance->m_pNodeWorldTransform->Transpose();
		m_pDeviceContext->UpdateSubresource(m_pTransformBuffer, 0, nullptr, &CB_Transform, 0, 0);

		// Draw
		meshInstance->Render(m_pDeviceContext);
	}
	m_StaticMeshInstance.clear();



	/*
		ImguiRender();
	*/

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Editor");

		//ImGui::Checkbox("AutoRot_Pitch", &isAutoRotatePitch);
		//ImGui::Checkbox("AutoRot_Yaw", &isAutoRotateYaw);
		//ImGui::Checkbox("AutoRot_Roll", &isAutoRotateRoll);

		//ImGui::Dummy(ImVec2(0.0f, 10.0f));
		//ImGui::Checkbox("NormalMap", &isNormalMap);
		//ImGui::Checkbox("SpecularMap", &isSpecularMap);
		//ImGui::Checkbox("Gamma_Correction", &isGamma);
		//ImGui::Checkbox("DiffuseMap", &isDiffuse);
		//ImGui::Checkbox("EmissiveMap", &isEmissive);
		//ImGui::Checkbox("OpacityMap", &isOpacity);
		//ImGui::Checkbox("MetalnessMap", &isMetalness);
		//ImGui::Checkbox("RoughnessMap", &isRoughness);

		//ImGui::Dummy(ImVec2(0.0f, 10.0f));
		//ImGui::SliderFloat3("Cam_Pos", m_Cam, -400.0f, 400.0f);
		//ImGui::SliderFloat3("Cube_Pos", m_Cb_Trans, -500.0f, 500.0f);
		//ImGui::SliderFloat3("Cube_Rot", m_Cb_Rot, -360.0f, 360.0f);
		//ImGui::SliderFloat("Cube_Scale", &m_Scale, 0.0f, 5.0f);

		//ImGui::Dummy(ImVec2(0.0f, 10.0f));
		//ImGui::SliderFloat("Far", &m_Far, 1.0f, 10000.0f);
		//ImGui::SliderFloat("Near", &m_Near, 0.01f, 10.0f);
		//ImGui::SliderFloat("Fov", &m_Fov, -20.0f, 180.0f);

		//ImGui::Dummy(ImVec2(0.0f, 10.0f));
		//ImGui::SliderFloat3("Light_RGB", m_vLightColor, 0.0f, 1.0f);
		//ImGui::SliderFloat3("Light_XYZ", m_vLightDir, -1.0f, 1.0f);

		//ImGui::Dummy(ImVec2(0.0f, 10.0f));
		//ImGui::SliderFloat("Specular", &m_SpecularPower, 1.0f, 500.0f);
		//ImGui::SliderFloat("Ambient", &m_Ambient, 0, 1);

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());



	// Present the information rendered to the back buffer to the front buffer (the screen)
	m_pSwapChain->Present(0, 0);
}

void D3DRenderManager::CreateSkeletalMesh_VS_IL()
{
	HRESULT hr = 0; // �����.

	ID3D10Blob* errorMessage = nullptr;	 // ���� �޽����� ������ ����.

	// 1. Render() ���� ���������ο� ���ε��� ���ؽ� ���۹� ���� ���� �غ�	
	// 2. Render() ���� ���������ο� ���ε��� InputLayout ���� 	 
	D3D_SHADER_MACRO defines[] =
	{
		{"VERTEX_SKINNING",""}, // ��ũ�� �̸��� ���� ����
		{nullptr, nullptr}    // �迭�� ���� ��Ÿ���� ���� nullptr�� ������.
	};
	ID3D10Blob* vertexShaderBuffer = nullptr;	// ���� ���̴� �ڵ尡 ����� ����.
	hr = CompileShaderFromFile(L"BasicVertexShader.hlsl", defines, "main", "vs_5_0", &vertexShaderBuffer);

	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"BasicVertexShader.cso", &vertexShaderBuffer);
	}
	HR_T(hr);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HR_T(m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pSkeletalMeshInputLayout));

	// 3. Render() ���� ���������ο� ���ε���  ���ؽ� ���̴� ����
	HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_pSkeletalMeshVertexShader));
	SAFE_RELEASE(vertexShaderBuffer);

}

void D3DRenderManager::CreateStaticMesh_VS_IL()
{
	HRESULT hr;
	// 2. Render() ���� ���������ο� ���ε��� InputLayout ���� 	
	D3D_SHADER_MACRO defines[] =
	{
		{"",""}, // ��ũ�� �̸��� ���� ����
		{nullptr, nullptr}    // �迭�� ���� ��Ÿ���� ���� nullptr�� ������.
	};
	ID3D10Blob* vertexShaderBuffer = nullptr;
	hr = CompileShaderFromFile(L"BasicVertexShader.hlsl", defines, "main", "vs_5_0", &vertexShaderBuffer);
	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"BasicVertexShader.cso", &vertexShaderBuffer);
	}
	HR_T(hr);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pStaticMeshInputLayout);

	// 3. Render() ���� ���������ο� ���ε���  ���ؽ� ���̴� ����
	HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_pStaticMeshVertexShader));
	SAFE_RELEASE(vertexShaderBuffer);
}

// 1. Render() ���� ���������ο� ���ε��� ���ؽ� ���۹� ���� ���� �غ�
// 2. Render() ���� ���������ο� ���ε��� InputLayout ���� 	
// 3. Render() ���� ���������ο� ���ε���  ���ؽ� ���̴� ����
// 4. Render() ���� ���������ο� ���ε��� �ε��� ���� ����
// 5. Render() ���� ���������ο� ���ε��� �ȼ� ���̴� ����
// 6. Render() ���� ���������ο� ���ε��� ��� ���� ����
void D3DRenderManager::CreatePS()
{
	HRESULT hr;
	// 4. Render() ���� ���������ο� ���ε��� �ε��� ���� ����
	// 5. Render() ���� ���������ο� ���ε��� �ȼ� ���̴� ����
	ID3D10Blob* pixelShaderBuffer = nullptr;	// �ȼ� ���̴� �ڵ尡 ����� ����.
	hr = CompileShaderFromFile(L"PBR_PixelShader.hlsl", nullptr, "main", "ps_5_0", &pixelShaderBuffer);
	if (FAILED(hr))
	{
		hr = D3DReadFileToBlob(L"PBR_PixelShader.cso", &pixelShaderBuffer);
	}
	HR_T(hr);

	HR_T(m_pDevice->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));
	SAFE_RELEASE(pixelShaderBuffer);
}

void D3DRenderManager::ApplyMaterial(Material* pMaterial)
{
	if (pMaterial->m_pDiffuseRV)
		m_pDeviceContext->PSSetShaderResources(0, 1, pMaterial->m_pDiffuseRV->m_pTextureSRV.GetAddressOf());
	if (pMaterial->m_pNormalRV)
		m_pDeviceContext->PSSetShaderResources(1, 1, pMaterial->m_pNormalRV->m_pTextureSRV.GetAddressOf());
	if (pMaterial->m_pSpecularRV)
		m_pDeviceContext->PSSetShaderResources(2, 1, pMaterial->m_pSpecularRV->m_pTextureSRV.GetAddressOf());
	if (pMaterial->m_pEmissiveRV)
		m_pDeviceContext->PSSetShaderResources(3, 1, pMaterial->m_pEmissiveRV->m_pTextureSRV.GetAddressOf());
	if (pMaterial->m_pOpacityRV)
		m_pDeviceContext->PSSetShaderResources(4, 1, pMaterial->m_pOpacityRV->m_pTextureSRV.GetAddressOf());
	if (pMaterial->m_pMetalnessRV)
		m_pDeviceContext->PSSetShaderResources(5, 1, pMaterial->m_pMetalnessRV->m_pTextureSRV.GetAddressOf());
	if (pMaterial->m_pRoughnessRV)
		m_pDeviceContext->PSSetShaderResources(6, 1, pMaterial->m_pRoughnessRV->m_pTextureSRV.GetAddressOf());
	 
	CB_Mat.Diffuse = pMaterial->m_Color;
	CB_Mat.UseDiffuseMap = pMaterial->m_pDiffuseRV != nullptr ? true : false;
	CB_Mat.UseNormalMap = pMaterial->m_pNormalRV != nullptr ? true : false;
	CB_Mat.UseSpecularMap = pMaterial->m_pSpecularRV != nullptr ? true : false;
	CB_Mat.UseEmissiveMap = pMaterial->m_pEmissiveRV != nullptr ? true : false;
	CB_Mat.UseOpacityMap = pMaterial->m_pOpacityRV != nullptr ? true : false;
	CB_Mat.m_pMetalnessRV = pMaterial->m_pMetalnessRV != nullptr ? true : false;
	CB_Mat.m_pRoughnessRV = pMaterial->m_pRoughnessRV != nullptr ? true : false;

	if (CB_Mat.UseOpacityMap && m_pAlphaBlendState != nullptr)
		m_pDeviceContext->OMSetBlendState(m_pAlphaBlendState, nullptr, 0xffffffff); // ���ĺ��� ���¼��� , �ٸ��ɼ��� �⺻�� 
	else
		m_pDeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);	// �������� , �ٸ��ɼ��� �⺻��

	m_pDeviceContext->UpdateSubresource(m_pMaterialBuffer, 0, nullptr, &CB_Mat, 0, 0);

}

void  D3DRenderManager::GetVideoMemoryInfo(std::string& out)
{
	DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo;
	m_pDXGIAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);

	out = std::to_string(videoMemoryInfo.CurrentUsage / 1024 / 1024) + " MB" + "/" + std::to_string(videoMemoryInfo.Budget / 1024 / 1024) + " MB";
}

void D3DRenderManager::GetSystemMemoryInfo(std::string& out)
{
	HANDLE hProcess = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS_EX);
	GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	out = std::to_string((pmc.PagefileUsage) / 1024 / 1024) + " MB";
}

