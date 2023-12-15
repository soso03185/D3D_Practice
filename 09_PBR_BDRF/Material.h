#pragma once

#include <wrl.h>
#include <directxtk\SimpleMath.h>
#include <d3d11.h>


using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace std;

struct aiMaterial;


class Material
{
public:
	Material();
	~Material();

	void Create(ID3D11Device* device, aiMaterial* pMaterial);

	ID3D11ShaderResourceView* m_pDiffuseRV = nullptr;	// ≈ÿΩ∫√≥ ∏Æº“Ω∫ ∫‰.
	ID3D11ShaderResourceView* m_pNormalRV = nullptr;	// ≥Î∏ª∏  ∏Æº“Ω∫ ∫‰.
	ID3D11ShaderResourceView* m_pSpecularRV = nullptr;	// Ω∫∆‰≈ß∑Ø∏  ∏Æº“Ω∫ ∫‰.
	ID3D11ShaderResourceView* m_pEmissiveRV = nullptr;	// ¿ÃπÃΩ√∫Í∏  ∏Æº“Ω∫ ∫‰.
	ID3D11ShaderResourceView* m_pOpacityRV = nullptr;	// ≈ı∏Ì∏  ∏Æº“Ω∫ ∫‰.	
	ID3D11ShaderResourceView* m_pMetalnessRV = nullptr;	// ∏ﬁ≈ª∏  ∏Æº“Ω∫ ∫‰.	
	ID3D11ShaderResourceView* m_pRoughnessRV = nullptr;	// ∑Ø«¡¥œΩ∫∏  ∏Æº“Ω∫ ∫‰.	
};

