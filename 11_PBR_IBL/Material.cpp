#include "../Common/pch.h"
#include "../Common/Helper.h"
#include "Material.h"
#include "D3DRenderManager.h"
#include "ResourceManager.h"

using namespace std;

Material::Material()
{

}

Material::~Material()
{
	
}


void Material::Create(aiMaterial* pMaterial)
{
	aiString texturePath;
	wstring basePath = L"../Resource/";
	std::filesystem::path path;
	wstring finalPath;
	string name = pMaterial->GetName().C_Str();

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		m_pDiffuseRV = ResourceManager::Instance->CreateMaterialTexture(finalPath);
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_NORMALS, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		m_pNormalRV = ResourceManager::Instance->CreateMaterialTexture(finalPath);
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		m_pSpecularRV = ResourceManager::Instance->CreateMaterialTexture(finalPath);
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		m_pEmissiveRV = ResourceManager::Instance->CreateMaterialTexture(finalPath);
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_OPACITY, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		m_pOpacityRV = ResourceManager::Instance->CreateMaterialTexture(finalPath);
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_METALNESS, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		m_pMetalnessRV = ResourceManager::Instance->CreateMaterialTexture(finalPath);
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_SHININESS, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		m_pRoughnessRV = ResourceManager::Instance->CreateMaterialTexture(finalPath);
	}
}

MaterialTexture::MaterialTexture()
{
}

MaterialTexture::~MaterialTexture()
{
}

void MaterialTexture::Create(const std::wstring& filePath)
{
	//ToDo :: DDS 말고 다른 파일형식도 가능하게끔 만들어야함.
	HR_T(CreateTextureFromFile(D3DRenderManager::m_pDevice, filePath.c_str(), &m_pTextureSRV));
	m_FilePath = filePath;
}
