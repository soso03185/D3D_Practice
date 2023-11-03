#include "../Common/pch.h"
#include "../Common/Helper.h"
#include "Material.h"

Material::Material()
{

}

Material::~Material()
{
	SAFE_RELEASE(m_pDiffuseRV);
	SAFE_RELEASE(m_pNormalRV);
	SAFE_RELEASE(m_pSpecularRV);
	SAFE_RELEASE(m_pEmissiveRV);
	SAFE_RELEASE(m_pOpacityRV);
}


void Material::Create(ID3D11Device* device, aiMaterial* pMaterial)
{
	string name = pMaterial->GetName().C_Str();

	wstring basePath = L"../Resource/";
	wstring finalPath;
	aiString texturePath;

	std::filesystem::path path;

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pDiffuseRV));
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_NORMALS, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pNormalRV));
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pSpecularRV));
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pEmissiveRV));
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_OPACITY, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pOpacityRV));
	}
}