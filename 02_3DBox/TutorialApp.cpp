#include "TutorialApp.h"
#include "../Common/Helper.h"
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

// ���� ����.
struct Vertex
{
	Vector3 position;		// ���� ��ġ ����.
	Vector4 color;			// ���� ���� ����.

	Vertex(float x, float y, float z) : position(x, y, z) { }
	Vertex(Vector3 position) : position(position) { }

	Vertex(Vector3 position, Vector4 color)
		: position(position), color(color) { }
};

struct ConstantBuffer
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
};


TutorialApp::TutorialApp(HINSTANCE hInstance)
	:GameApp(hInstance)
{

}

TutorialApp::~TutorialApp()
{
	UninitScene();
	UninitD3D();
	UninitImGUI();
}

bool TutorialApp::Initialize(UINT Width, UINT Height)
{
	__super::Initialize(Width, Height);

	if (!InitD3D())
		return false;

	if (!InitImGUI())
		return false;

	if (!InitScene())
		return false;

	return true;
}

void TutorialApp::Update()
{
	__super::Update();

	float t = GameTimer::m_Instance->TotalTime();

	// 1st Cube: Rotate around the origin
	XMMATRIX mSpin = XMMatrixRotationY(t * m_Cb_speed[0]);
	XMMATRIX mTranslate = XMMatrixTranslation(m_Cb1_tr[0], m_Cb1_tr[1], m_Cb1_tr[2]);
	m_World1 = mSpin * mTranslate;

	// 2nd Cube:  Rotate around origin
	XMMATRIX mSpin2 = XMMatrixRotationY(-t * m_Cb_speed[1]);
	XMMATRIX mTranslate2 = XMMatrixTranslation(m_Cb2_tr[0], m_Cb2_tr[1], m_Cb2_tr[2]);
	XMMATRIX mScale2 = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	m_World2 = mScale2 * mSpin2 * mTranslate2;

	// 3rd Cube:  Rotate around origin
	XMMATRIX mSpin3 = XMMatrixRotationY(-t * m_Cb_speed[2]);
	XMMATRIX mTranslate3 = XMMatrixTranslation(m_Cb3_tr[0], m_Cb3_tr[1], m_Cb3_tr[2]);
	XMMATRIX mScale3 = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	m_World3 = mScale3 * mSpin3 * mTranslate3;

	// Left Cube
	m_World4 = XMMatrixRotationY(t * 4.0f) * XMMatrixRotationZ(-t * 4.0f);

	// Cam Transform
	m_Eye = XMVectorSet(m_Cam[0], m_Cam[1], m_Cam[2], 0.0f);
	m_At = XMVectorSet(m_Cam[0], m_Cam[1], 0.0f, 0.0f);
	m_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_View = XMMatrixLookAtLH(m_Eye, m_At, m_Up);

	m_Projection = XMMatrixPerspectiveFovLH(m_Fov / 180.0f * 3.14f, m_ClientWidth / (FLOAT)m_ClientHeight, m_Near, m_Far);

}

void TutorialApp::ImguiRender()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	D3D11_VIEWPORT viewports[2]; // �� ���� ����Ʈ�� ������ �迭 ����
	viewports[0] = viewport1;
	viewports[1] = viewport2;
	m_pDeviceContext->RSSetViewports(2, viewports);

	{
		ImGui::Begin("Editor");
		ImGui::SliderFloat3("Cube1_Pos", m_Cb1_tr, -20.0f, 20.0f);
		ImGui::SliderFloat3("Cube2_Pos", m_Cb2_tr, -20.0f, 20.0f);
		ImGui::SliderFloat3("Cube3_Pos", m_Cb3_tr, -20.0f, 20.0f);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat3("Cube_Speed", m_Cb_speed, -20.0f, 20.0f);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat3("CamPos", m_Cam, -20.0f, 20.0f);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat("Fov", &m_Fov, -20.0f, 180.0f);
		ImGui::SliderFloat("Near", &m_Near, 0.01f, 10.0f);
		ImGui::SliderFloat("Far", &m_Far, 1.0f, 100.0f);

		//	ImGui::ColorEdit3("clear color", (float*)&m_ClearColor); 
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void TutorialApp::LeftRender()
{
	// ù ��° ����Ʈ�� ���� ������ �۾� ����
	m_pDeviceContext->RSSetViewports(1, &viewport1);

	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer2, DXGI_FORMAT_R16_UINT, 0);
	ConstantBuffer bg;
	bg.mWorld = XMMatrixScaling(10, 10, 1);
	bg.mView = XMMatrixTranspose(m_View);
	bg.mProjection = XMMatrixTranspose(m_Projection);

	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &bg, 0, 0);
	// ȭ�� ĥ�ϱ�.
	m_pDeviceContext->DrawIndexed(m_nIndices2, 0, 0);

	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Update variables for the first cube
	ConstantBuffer Lcb1;
	Lcb1.mWorld = XMMatrixTranspose(m_World4);
	Lcb1.mView = XMMatrixTranspose(m_View);
	Lcb1.mProjection = XMMatrixTranspose(m_Projection);
	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &Lcb1, 0, 0);

	// ȭ�� ĥ�ϱ�.
	m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);
}

void TutorialApp::RightRender()
{
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// �� ��° ����Ʈ�� ���� ������ �۾� ����
	m_pDeviceContext->RSSetViewports(1, &viewport2);

	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Update variables for the first cube
	ConstantBuffer Rcb1;
	Rcb1.mWorld = XMMatrixTranspose(m_World1);
	Rcb1.mView = XMMatrixTranspose(m_View);
	Rcb1.mProjection = XMMatrixTranspose(m_Projection);
	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &Rcb1, 0, 0);

	// ȭ�� ĥ�ϱ�.
	m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);

	// Update variables for the second cube	
	ConstantBuffer cb2;
	cb2.mWorld = XMMatrixTranspose(m_World2 * m_World1);
	cb2.mView = XMMatrixTranspose(m_View);
	cb2.mProjection = XMMatrixTranspose(m_Projection);
	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb2, 0, 0);

	m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);

	// Update variables for the third cube	
	ConstantBuffer cb3;
	cb3.mWorld = XMMatrixTranspose(m_World3 * m_World2 * m_World1);
	cb3.mView = XMMatrixTranspose(m_View);
	cb3.mProjection = XMMatrixTranspose(m_Projection);
	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb3, 0, 0);
	// ȭ�� ĥ�ϱ�.
	m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);
}

void TutorialApp::Render()
{
	// Draw�迭 �Լ��� ȣ���ϱ����� ������ ���������ο� �ʼ� �������� ������ �ؾ��Ѵ�	
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // ������ �̾ �׸� ��� ����.
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	// INDEX���� ����
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	LeftRender();
	RightRender();
	ImguiRender();

	// Present the information rendered to the back buffer to the front buffer (the screen)
	m_pSwapChain->Present(0, 0);
}

bool TutorialApp::InitImGUI()
{
	/*
		ImGui �ʱ�ȭ.
	*/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(this->m_pDevice, this->m_pDeviceContext);
	return true;
}

void TutorialApp::UninitImGUI()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool TutorialApp::InitD3D()
{
	// �����.
	HRESULT hr = 0;

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

	// ���� Ÿ���� ���� ��� ���������ο� ���ε��մϴ�.
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);


	// ����� �ػ󵵸� ���´ٰ� �����մϴ�.
	int monitorWidth = (float)m_ClientWidth;  // ������� ���� �ػ�
	int monitorHeight = (float)m_ClientHeight;  // ������� ���� �ػ�


	// ù ��° ����Ʈ: ���� ���� ȭ��
	viewport1 = {};
	viewport1.TopLeftX = 0.0f;
	viewport1.TopLeftY = 0.0f;
	viewport1.Width = monitorWidth / 3.0f;
	viewport1.Height = monitorHeight;
	viewport1.MinDepth = 0.0f;
	viewport1.MaxDepth = 1.0f;

	// �� ��° ����Ʈ: ������ ���� ȭ��
	viewport2 = {};
	viewport2.TopLeftX = monitorWidth / 3.0f;
	viewport2.TopLeftY = 0.0f;
	viewport2.Width = (monitorWidth * 2) / 3.0f;
	viewport2.Height = monitorHeight;
	viewport2.MinDepth = 0.0f;
	viewport2.MaxDepth = 1.0f;

	//6. depth & stencil view ����
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
	return true;
}

void TutorialApp::UninitD3D()
{
	// Cleanup DirectX
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pRenderTargetView);
}

// 1. Render() ���� ���������ο� ���ε��� ���ؽ� ���۹� ���� ���� �غ�
// 2. Render() ���� ���������ο� ���ε��� InputLayout ���� 	
// 3. Render() ���� ���������ο� ���ε���  ���ؽ� ���̴� ����
// 4. Render() ���� ���������ο� ���ε��� �ε��� ���� ����
// 5. Render() ���� ���������ο� ���ε��� �ȼ� ���̴� ����
// 6. Render() ���� ���������ο� ���ε��� ��� ���� ����
bool TutorialApp::InitScene()
{
	HRESULT hr = 0; // �����.
	ID3D10Blob* errorMessage = nullptr;	 // ���� �޽����� ������ ����.

	// 1. Render() ���� ���������ο� ���ε��� ���ؽ� ���۹� ���� ���� �غ�
	// Local or Object or Model Space
	Vertex vertices[] =
	{
		// Cube
		{ Vector3(-1.0f, 1.0f, -1.0f),	Vector4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, -1.0f),	Vector4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, 1.0f),	Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(-1.0f, 1.0f, 1.0f),	Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ Vector3(-1.0f, -1.0f, -1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ Vector3(1.0f, -1.0f, -1.0f),	Vector4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ Vector3(1.0f, -1.0f, 1.0f),	Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(-1.0f, -1.0f, 1.0f),	Vector4(1.0f, 0.0f, 0.0f, 1.0f) },

		// BackGround
		{ Vector3(-1.0f, 1.0f, 1.0f),		Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, 1.0f),		Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ Vector3(-1.0f, -1.0f, 1.0f),	Vector4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(1.0f, -1.0f, 1.0f),		Vector4(0.0f, 1.0f, 1.0f, 1.0f) }
	};

	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices; // �迭 ������ �Ҵ�.
	HR_T(m_pDevice->CreateBuffer(&bd, &vbData, &m_pVertexBuffer));

	m_VertexBufferStride = sizeof(Vertex); // ���ؽ� ������ ����
	m_VertexBufferOffset = 0;


	// 2. Render() ���� ���������ο� ���ε��� InputLayout ���� 	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ID3D10Blob* vertexShaderBuffer = nullptr;	// ���� ���̴� �ڵ尡 ����� ����.
	HR_T(CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer));
	HR_T(m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pInputLayout));

	// 3. Render() ���� ���������ο� ���ε���  ���ؽ� ���̴� ����
	HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader));
	SAFE_RELEASE(vertexShaderBuffer);

	// 4. Render() ���� ���������ο� ���ε��� �ε��� ���� ����
	WORD indices[] =
	{
		3,1,0, 2,1,3,
		0,5,4, 1,5,0,
		3,4,7, 0,4,3,
		1,6,5, 2,6,1,
		2,7,6, 3,7,2,
		6,4,5, 7,4,6,
	};

	WORD BGindices[] =
	{
		8,9,10,
		10,9,11,
	};

	// Cube �ε��� ���� ����.
	m_nIndices = ARRAYSIZE(indices);
	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices;
	HR_T(m_pDevice->CreateBuffer(&bd, &ibData, &m_pIndexBuffer));

	// BG �ε��� ���� ����.
	m_nIndices2 = ARRAYSIZE(BGindices);
	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(BGindices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData2 = {};
	ibData2.pSysMem = BGindices;
	HR_T(m_pDevice->CreateBuffer(&bd, &ibData2, &m_pIndexBuffer2));

	// 5. Render() ���� ���������ο� ���ε��� �ȼ� ���̴� ����
	ID3D10Blob* pixelShaderBuffer = nullptr;	// �ȼ� ���̴� �ڵ尡 ����� ����.
	HR_T(CompileShaderFromFile(L"BasicPixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer));
	HR_T(m_pDevice->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));
	SAFE_RELEASE(pixelShaderBuffer);

	// 6. Render() ���� ���������ο� ���ε��� ��� ���� ����
	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer));

	// ���̴��� ������ ������ ����
	// Initialize the world matrix
	m_World1 = XMMatrixIdentity();
	m_World2 = XMMatrixIdentity();
	m_World3 = XMMatrixIdentity();
	m_World4 = XMMatrixIdentity();

	//// Initialize the view matrix
	//XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);  
	//XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); 
	//XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_Eye = XMVectorSet(m_Cam[0], m_Cam[1] = 1.0f, m_Cam[2] = -5.0f, 0.0f);
	m_At = XMVectorSet(m_Cam[0], m_Cam[1] = 1.0f, 0.0f, 0.0f);
	m_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_View = XMMatrixLookAtLH(m_Eye, m_At, m_Up);

	// Initialize the projection matrix  	// fov
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);
	
	m_Fov = 100.0f;
	m_Near = 0.01f;
	m_Far = 100.0f;
	return true;
}

void TutorialApp::UninitScene()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pConstantBuffer);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK TutorialApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}
