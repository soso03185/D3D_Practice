#pragma once
#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

enum class Mode
{
	Off,
	Write,
	Mask
};

class Stencil
{
public:
	Stencil() {};

	void Init(ID3D11Device* device, Mode mode, int depthIndex = 0)
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };
		m_DepthIndex = depthIndex;

		if (mode == Mode::Write) // ���ٽ� �׽�Ʈ�� ��ġ�� �ʰ� �׻� �ȼ��� �������ϰ� ���� ��
		{
			dsDesc.StencilEnable = true;
			dsDesc.StencilWriteMask = m_DepthIndex; // 0xff �̰ſ����� �켱 �ٲ�
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		}
		else if (mode == Mode::Mask) 
		{
			dsDesc.DepthEnable = false;
			dsDesc.StencilEnable = true;
			dsDesc.StencilReadMask = m_DepthIndex; // 0xff 
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		}
		device->CreateDepthStencilState(&dsDesc, &pStencil);
	}

	void Bind(ID3D11DeviceContext* deviceContext)
	{
		deviceContext->OMSetDepthStencilState(pStencil.Get(), m_DepthIndex);
	}

	int m_DepthIndex;
	ComPtr<ID3D11DepthStencilState> pStencil;
};

