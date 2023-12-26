#pragma once

#include "D3DRenderManager.h"

#include <wrl.h>
#include <directxtk\SimpleMath.h>
#include <d3d11.h>
#include <memory>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

struct aiMaterial;

class MaterialTexture
{
public:
	MaterialTexture();
	~MaterialTexture();

	std::wstring m_FilePath;
	ComPtr<ID3D11ShaderResourceView> m_pTextureSRV = nullptr;	// �ؽ�ó ���ҽ� ��.
	void Create(const std::wstring& filePath);
};


class Material
{
public:
	Material();
	~Material();

	void Create(aiMaterial* pMaterial);

	Vector4 m_Color;

	std::shared_ptr<MaterialTexture> m_pDiffuseRV = nullptr;	// �ؽ�ó ���ҽ� ��.
	std::shared_ptr<MaterialTexture> m_pNormalRV = nullptr;	// �븻�� ���ҽ� ��.
	std::shared_ptr<MaterialTexture> m_pSpecularRV = nullptr;	// ����ŧ���� ���ҽ� ��.
	std::shared_ptr<MaterialTexture> m_pEmissiveRV = nullptr;	// �̹̽ú�� ���ҽ� ��.
	std::shared_ptr<MaterialTexture> m_pOpacityRV = nullptr;	// ����� ���ҽ� ��.	
	std::shared_ptr<MaterialTexture> m_pMetalnessRV = nullptr;	// ��Ż�� ���ҽ� ��.	
	std::shared_ptr<MaterialTexture> m_pRoughnessRV = nullptr;	// �����Ͻ��� ���ҽ� ��.	
};

