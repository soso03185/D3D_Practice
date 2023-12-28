#include "TutorialApp.h"
#include "../Common/Helper.h"
#include "../Common/GameApp.h"
#include "Model.h"
#include "Node.h"
#include "D3DRenderManager.h"
#include "ResourceManager.h"

#include <d3dcompiler.h>
#include <directxtk/WICTextureLoader.h>
// #include <directxtk\Mouse.h>
// #include <directxtk\Keyboard.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib,"d3dcompiler.lib")

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp/scene.h>

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

	if (!InitD3D())		return false;
	if (!InitImGUI())	return false;
	if (!InitScene())	return false;

	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_previousTime);
	QueryPerformanceCounter(&m_currentTime);
	
	D3DRenderManager::m_pDevice = m_pDevice;

	return true;
}

void TutorialApp::Update()
{
	__super::Update();

	float t = GameTimer::m_Instance->TotalTime() / 4;

	// Right
	XMMATRIX mScale = XMMatrixScaling(m_Scale, m_Scale, m_Scale);
	XMMATRIX mSpinX = XMMatrixRotationX(t * m_Cb_speed[0]);
	XMMATRIX mSpinY = XMMatrixRotationY(t * m_Cb_speed[0]);
	XMMATRIX mSpinZ = XMMatrixRotationZ(t * m_Cb_speed[0]);
	XMMATRIX mTranslate = XMMatrixTranslation(m_Cb_Trans[0], m_Cb_Trans[1], m_Cb_Trans[2]);

	if (isAutoRotateRoll == false)
	{
		float radian = XMConvertToRadians(m_Cb_Rot[2]);
		mSpinZ = XMMatrixRotationZ(radian);
	}

	if (isAutoRotatePitch == false)
	{
		float radian = XMConvertToRadians(m_Cb_Rot[0]);
		mSpinX = XMMatrixRotationX(radian);
	}

	if (isAutoRotateYaw == false)
	{
		float radian = XMConvertToRadians(m_Cb_Rot[1]);
		mSpinY = XMMatrixRotationY(radian);
	}

	m_World = mScale * mSpinZ * mSpinX * mSpinY * mTranslate;

	// Cam Transform
	m_Eye = XMVectorSet(m_Cam[0], m_Cam[1], m_Cam[2], 0.0f);
	m_At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
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

	m_pModel->Update(m_deltaTime);
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

	m_pDeviceContext->RSSetViewports(1, &viewport);

	{
		ImGui::Begin("Editor");

		ImGui::Checkbox("AutoRot_Pitch", &isAutoRotatePitch);
		ImGui::Checkbox("AutoRot_Yaw", &isAutoRotateYaw);
		ImGui::Checkbox("AutoRot_Roll", &isAutoRotateRoll);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Checkbox("NormalMap", &isNormalMap);
		ImGui::Checkbox("SpecularMap", &isSpecularMap);
		ImGui::Checkbox("Gamma_Correction", &isGamma);
		ImGui::Checkbox("DiffuseMap", &isDiffuse);
		ImGui::Checkbox("EmissiveMap", &isEmissive);
		ImGui::Checkbox("OpacityMap", &isOpacity);
		ImGui::Checkbox("MetalnessMap", &isMetalness);
		ImGui::Checkbox("RoughnessMap", &isRoughness);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat3("Cam_Pos", m_Cam, -400.0f, 400.0f);
		ImGui::SliderFloat3("Cube_Pos", m_Cb_Trans, -500.0f, 500.0f);
		ImGui::SliderFloat3("Cube_Rot", m_Cb_Rot, -360.0f, 360.0f);
		ImGui::SliderFloat("Cube_Scale", &m_Scale, 0.0f, 5.0f);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat("Far", &m_Far, 1.0f, 10000.0f);
		ImGui::SliderFloat("Near", &m_Near, 0.01f, 10.0f);
		ImGui::SliderFloat("Fov", &m_Fov, -20.0f, 180.0f);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat3("Light_RGB", m_vLightColor, 0.0f, 1.0f);
		ImGui::SliderFloat3("Light_XYZ", m_vLightDir, -1.0f, 1.0f);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat("Specular", &m_SpecularPower, 1.0f, 500.0f);
		ImGui::SliderFloat("Ambient", &m_Ambient, 0, 1);

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void TutorialApp::ModelRender()
{
	///  ConstantBuffer Binding  ///
	CB_ConstantBuffer CB_Buff;
	CB_Buff.mAmbient = m_Ambient;
	CB_Buff.mSpecularPower = m_SpecularPower;

	CB_BoolBuffer CB_Bool;
	CB_Bool.UseGamma = isGamma;

	CB_TransformBuffer CB_Transform;
	CB_Transform.mWorld = XMMatrixTranspose(m_World);
	CB_Transform.mView = XMMatrixTranspose(m_View);
	CB_Transform.mProjection = XMMatrixTranspose(m_Projection);

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

	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &CB_Buff, 0, 0);
	m_pDeviceContext->UpdateSubresource(m_pLightBuffer, 0, nullptr, &CB_Light, 0, 0);

	for (size_t i = 0; i < m_pModel->m_Meshes.size(); i++)
	{
		size_t mi = m_pModel->m_Meshes[i].m_MaterialIndex;
		
		if(m_pModel->m_Materials[mi].m_pDiffuseRV)
			m_pDeviceContext->PSSetShaderResources(0, 1, m_pModel->m_Materials[mi].m_pDiffuseRV->m_pTextureSRV.GetAddressOf());
		if (m_pModel->m_Materials[mi].m_pNormalRV)
			m_pDeviceContext->PSSetShaderResources(1, 1, m_pModel->m_Materials[mi].m_pNormalRV->m_pTextureSRV.GetAddressOf());
		if (m_pModel->m_Materials[mi].m_pSpecularRV)
			m_pDeviceContext->PSSetShaderResources(2, 1, m_pModel->m_Materials[mi].m_pSpecularRV->m_pTextureSRV.GetAddressOf());
		if (m_pModel->m_Materials[mi].m_pEmissiveRV)
			m_pDeviceContext->PSSetShaderResources(3, 1, m_pModel->m_Materials[mi].m_pEmissiveRV->m_pTextureSRV.GetAddressOf());
		if (m_pModel->m_Materials[mi].m_pOpacityRV)
			m_pDeviceContext->PSSetShaderResources(4, 1, m_pModel->m_Materials[mi].m_pOpacityRV->m_pTextureSRV.GetAddressOf());
		if (m_pModel->m_Materials[mi].m_pMetalnessRV)
			m_pDeviceContext->PSSetShaderResources(5, 1, m_pModel->m_Materials[mi].m_pMetalnessRV->m_pTextureSRV.GetAddressOf());
		if (m_pModel->m_Materials[mi].m_pRoughnessRV)
			m_pDeviceContext->PSSetShaderResources(6, 1, m_pModel->m_Materials[mi].m_pRoughnessRV->m_pTextureSRV.GetAddressOf());

		CB_Bool.UseDiffuseMap = m_pModel->m_Materials[mi].m_pDiffuseRV != nullptr ? isDiffuse : false;
		CB_Bool.UseNormalMap = m_pModel->m_Materials[mi].m_pNormalRV != nullptr ? isNormalMap : false;
		CB_Bool.UseSpecularMap = m_pModel->m_Materials[mi].m_pSpecularRV != nullptr ? isSpecularMap : false;
		CB_Bool.UseEmissiveMap = m_pModel->m_Materials[mi].m_pEmissiveRV != nullptr ? isEmissive : false;
		CB_Bool.UseOpacityMap = m_pModel->m_Materials[mi].m_pOpacityRV != nullptr ? isOpacity : false;
		CB_Bool.UseMetalnessMap = m_pModel->m_Materials[mi].m_pMetalnessRV != nullptr ? isMetalness : false;
		CB_Bool.UseRoughnessMap = m_pModel->m_Materials[mi].m_pRoughnessRV != nullptr ? isRoughness : false;

		if (CB_Bool.UseOpacityMap)	// 알파블렌드 상태설정 , 다른옵션은 기본값 
			m_pDeviceContext->OMSetBlendState(m_pAlphaBlendState, nullptr, 0xffffffff);
		else	// 설정해제 , 다른옵션은 기본값
			m_pDeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

		// 내가 추가한 코드 
		CB_Transform.mWorld = XMMatrixTranspose(m_World) * XMMatrixTranspose(*(m_pModel->m_Meshes[i].m_pNodeWorld));

		CB_MatrixPalette CB_MatPalatte;
		m_pModel->m_Meshes[i].UpdateMatrixPalette(CB_MatPalatte.Array);
		m_pDeviceContext->UpdateSubresource(m_pMatPalette, 0, nullptr, &CB_MatPalatte, 0, 0);
		m_pDeviceContext->UpdateSubresource(m_pTransformBuffer, 0, nullptr, &CB_Transform, 0, 0);
		m_pDeviceContext->UpdateSubresource(m_pBoolBuffer, 0, nullptr, &CB_Bool, 0, 0);

		m_pDeviceContext->IASetIndexBuffer(m_pModel->m_Meshes[i].m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		m_pDeviceContext->IASetVertexBuffers
		(
			0, 1,
			&m_pModel->m_Meshes[i].m_pVertexBuffer,
			//	&m_pModel->m_Meshes[i].m_pBWVertexBuffer,
			&m_pModel->m_Meshes[i].m_VertexBufferStride,
			&m_pModel->m_Meshes[i].m_VertexBufferOffset
		);

		m_pDeviceContext->DrawIndexed(m_pModel->m_Meshes[i].m_IndexCount, 0, 0);
	}
}

void TutorialApp::Render()
{
	// Draw계열 함수를 호출하기전에 렌더링 파이프라인에 필수 스테이지 설정을 해야한다	
	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	// vertex shader
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pBoolBuffer);
	m_pDeviceContext->VSSetConstantBuffers(2, 1, &m_pTransformBuffer);
	m_pDeviceContext->VSSetConstantBuffers(3, 1, &m_pLightBuffer);
	m_pDeviceContext->VSSetConstantBuffers(4, 1, &m_pMatPalette);

	// pixel shader
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pBoolBuffer);
	m_pDeviceContext->PSSetConstantBuffers(2, 1, &m_pTransformBuffer);
	m_pDeviceContext->PSSetConstantBuffers(3, 1, &m_pLightBuffer);
	m_pDeviceContext->PSSetConstantBuffers(4, 1, &m_pMatPalette);

	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
	m_pDeviceContext->RSSetViewports(1, &viewport);

	ModelRender();
	ImguiRender();

	// Present the information rendered to the back buffer to the front buffer (the screen)
	m_pSwapChain->Present(0, 0);
}

bool TutorialApp::InitImGUI()
{
	/*
		ImGui 초기화.
	*/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

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
	// 결과값.
	HRESULT hr = 0;

	// 스왑체인 속성 설정 구조체 생성.
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;	// 스왑체인 출력할 창 핸들 값.
	swapDesc.Windowed = true;		// 창 모드 여부 설정.
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 백버퍼(텍스처)의 가로/세로 크기 설정.
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Height = m_ClientHeight;
	// 화면 주사율 설정.
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// 샘플링 관련 설정.
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	UINT creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 1. 장치 생성.   2.스왑체인 생성. 3.장치 컨텍스트 생성.
	HR_T(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL,
		D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext));

	// 4. 렌더타겟뷰 생성.  (백버퍼를 이용하는 렌더타겟뷰)	
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	HR_T(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView));  // 텍스처는 내부 참조 증가

	SAFE_RELEASE(pBackBufferTexture);	//외부 참조 카운트를 감소시킨다.

	// 모니터 해상도를 얻어온다고 가정합니다.
	int monitorWidth = (float)m_ClientWidth;  // 모니터의 가로 해상도
	int monitorHeight = (float)m_ClientHeight;  // 모니터의 세로 해상도

	viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = monitorWidth;
	viewport.Height = monitorHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// 6. depth & stencil view 생성
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


	// 7. 알파블렌딩을 위한 블렌드 상태 생성
	// FinalAlpha = (SrcAlpha * SrcBlendAlpha) + (DestAlpha * DestBlendAlpha)
	// FinalRGB = (SrcRGB * SrcBlend) + (DestRGB * DestBlend)

	D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
	rtBlendDesc.BlendEnable = true;						// 블렌드 사용 여부
	rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;	    // SrcBlend는 SrcColor의 알파값
	rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	// DestBlend는 (1-SourceColor.a)
	rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;			// 컬러 블렌딩 연산을 정의합니다.
	rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;		// 알파 블렌딩 연산을 정의합니다.
	rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;		// SrcBlendAlpha = 1
	rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;		// DestBlendAlpha = 1	
	rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // 렌더타겟에 RGBA 모두 Write	// 현재 렌더 타겟에 대한 컬러 컴포넌트 쓰기 마스크를 정의합니다.

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;  // 다중 샘플링 앤틸리어싱(Anti-aliasing)을 지원하기 위해 사용
	blendDesc.IndependentBlendEnable = false; // 각 렌더 타겟에 대한 독립적인 블렌딩 설정을 사용할지 여부
	blendDesc.RenderTarget[0] = rtBlendDesc;
	HR_T(m_pDevice->CreateBlendState(&blendDesc, &m_pAlphaBlendState));

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

// 1. Render() 에서 파이프라인에 바인딩할 버텍스 버퍼및 버퍼 정보 준비
// 2. Render() 에서 파이프라인에 바인딩할 InputLayout 생성 	
// 3. Render() 에서 파이프라인에 바인딩할  버텍스 셰이더 생성
// 4. Render() 에서 파이프라인에 바인딩할 인덱스 버퍼 생성
// 5. Render() 에서 파이프라인에 바인딩할 픽셀 셰이더 생성
// 6. Render() 에서 파이프라인에 바인딩할 상수 버퍼 생성
bool TutorialApp::InitScene()
{
	HRESULT hr = 0; // 결과값.
	ID3D10Blob* errorMessage = nullptr;	 // 에러 메시지를 저장할 버퍼.

	// 1. Render() 에서 파이프라인에 바인딩할 버텍스 버퍼및 버퍼 정보 준비	
	// 2. Render() 에서 파이프라인에 바인딩할 InputLayout 생성 	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ID3D10Blob* vertexShaderBuffer = nullptr;	// 정점 셰이더 코드가 저장될 버퍼.
	HR_T(CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_5_0", &vertexShaderBuffer));
	HR_T(m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pInputLayout));

	// 3. Render() 에서 파이프라인에 바인딩할  버텍스 셰이더 생성
	HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader));
	SAFE_RELEASE(vertexShaderBuffer);

	// 4. Render() 에서 파이프라인에 바인딩할 인덱스 버퍼 생성
	// 5. Render() 에서 파이프라인에 바인딩할 픽셀 셰이더 생성
	ID3D10Blob* pixelShaderBuffer = nullptr;	// 픽셀 셰이더 코드가 저장될 버퍼.
	//	HR_T(CompileShaderFromFile(L"BasicPixelShader.hlsl", "main", "ps_5_0", &pixelShaderBuffer));
	HR_T(CompileShaderFromFile(L"PBR_PixelShader.hlsl", "main", "ps_5_0", &pixelShaderBuffer));
	HR_T(m_pDevice->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));
	SAFE_RELEASE(pixelShaderBuffer);


	// 6. Render() 에서 파이프라인에 바인딩할 상수 버퍼 생성     Create the constant buffer 
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


	// 7. Render() 에서 파이프라인에 바인딩할 쉐이더 리소스와 샘플러 생성 (텍스처 로드 & sample state 생성 )
	// 8. FBX Load	
	m_pModel = new Model();
//	m_pModel->ReadFile(m_pDevice, "../Resource/cerberus2.fbx");


	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR_T(m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear));

	// 쉐이더에 전달할 데이터 설정
	// Initialize the world matrix
	m_World = XMMatrixIdentity();
	m_Eye = XMVectorSet(m_Cam[0], m_Cam[1], m_Cam[2], 0.0f);
	m_At = XMVectorSet(m_Cam[0], m_Cam[1], 0.0f, 0.0f);
	m_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_View = XMMatrixLookAtLH(m_Eye, m_At, m_Up);

	// Initialize the projection matrix  	// fov
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);

	return true;
}

void TutorialApp::UninitScene()
{

	//  이거 하면 끌때 mesh 해제해주는거에서 오류생김
	//	delete m_pModel;
	//	delete m_pNode;

	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pBoolBuffer);
	SAFE_RELEASE(m_pTransformBuffer);
	SAFE_RELEASE(m_pLightBuffer);
	SAFE_RELEASE(m_pAlphaBlendState);
	SAFE_RELEASE(m_pMatPalette);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK TutorialApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_UP) {			
			m_pModel->ReadFile(m_pDevice,"../Resource/cerberus2.fbx");
			//IncreaseModel();
		}
		else if (wParam == VK_DOWN) {
			//DecreaseModel();
		}
		break;
	}

	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}
