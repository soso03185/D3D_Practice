#pragma once

#include "D3DRenderManager.h"

#include <wrl.h>
#include <directxtk\SimpleMath.h>
#include <d3d11.h>
#include <memory>
#include <string>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

struct aiMaterial;


	class MaterialTexture
	{
	public:
		MaterialTexture();
		~MaterialTexture();

		std::wstring m_FilePath;
		ComPtr<ID3D11ShaderResourceView> m_pTextureSRV = nullptr;	// ≈ÿΩ∫√≥ ∏Æº“Ω∫ ∫‰.
		void Create(const std::wstring& filePath);
	};

	
	class Material
	{
	public:
		Material();
		~Material();

		void Create(aiMaterial* pMaterial);

		Vector4 m_Color;

		std::shared_ptr<MaterialTexture> m_pDiffuseRV = nullptr;	// ≈ÿΩ∫√≥ ∏Æº“Ω∫ ∫‰.
		std::shared_ptr<MaterialTexture> m_pNormalRV = nullptr;	    // ≥Î∏ª∏  ∏Æº“Ω∫ ∫‰.
		std::shared_ptr<MaterialTexture> m_pSpecularRV = nullptr;	// Ω∫∆‰≈ß∑Ø∏  ∏Æº“Ω∫ ∫‰.
		std::shared_ptr<MaterialTexture> m_pEmissiveRV = nullptr;	// ¿ÃπÃΩ√∫Í∏  ∏Æº“Ω∫ ∫‰.
		std::shared_ptr<MaterialTexture> m_pOpacityRV = nullptr;	// ≈ı∏Ì∏  ∏Æº“Ω∫ ∫‰.	
		std::shared_ptr<MaterialTexture> m_pMetalnessRV = nullptr;	// ∏ﬁ≈ª∏  ∏Æº“Ω∫ ∫‰.	
		std::shared_ptr<MaterialTexture> m_pRoughnessRV = nullptr;	// ∑Ø«¡¥œΩ∫∏  ∏Æº“Ω∫ ∫‰.	
	};
	