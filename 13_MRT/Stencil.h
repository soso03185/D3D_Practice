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

	void Init(ID3D11Device* device, Mode mode)
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

		if (mode == Mode::Write)
		{
			dsDesc.StencilEnable = true;
			dsDesc.StencilWriteMask = 0xff;
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		}
		else if (mode == Mode::Mask) // OutLine
		{
			dsDesc.DepthEnable = false;
			dsDesc.StencilEnable = true;
			dsDesc.StencilReadMask = 0xff;
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		}
		device->CreateDepthStencilState(&dsDesc, &pStencil);
	}

	void Bind(ID3D11DeviceContext* deviceContext)
	{
		deviceContext->OMSetDepthStencilState(pStencil.Get(), 0xff);
	}

	ComPtr<ID3D11DepthStencilState> pStencil;
};

