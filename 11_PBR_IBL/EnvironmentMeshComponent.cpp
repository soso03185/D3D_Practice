
#include "ResourceManager.h"
#include "D3DRenderManager.h"
#include "EnvironmentMeshComponent.h"
#include "Mesh.h"

EnvironmentMeshComponent::EnvironmentMeshComponent()
{
//	D3DRenderManager::Instance->m_StaticMeshComponents.push_back(this);
//	m_iterator = --D3DRenderManager::Instance->m_StaticMeshComponents.end();
}

EnvironmentMeshComponent::~EnvironmentMeshComponent()
{
}

bool EnvironmentMeshComponent::ReadEnvironmentMeshFromFBX(std::string filePath)
{
	// FBX ���� �б�	
	m_SceneResource = ResourceManager::Instance->CreateModelResource(filePath, ModelType::STATIC);

	assert(m_SceneResource != nullptr);
	if (m_SceneResource == nullptr)
	{
		return false;
	}
	m_MeshInstance.Create(&m_SceneResource->m_Meshes[0], &m_World, nullptr);
	m_SceneFilePath = filePath;
	return true;
}

bool EnvironmentMeshComponent::ReadEnvironmentTextureFromDDS(std::wstring filePath)
{
	m_EnvironmentTextureResource = ResourceManager::Instance->CreateTextureResource(filePath);
	assert(m_EnvironmentTextureResource != nullptr);
	if (m_EnvironmentTextureResource == nullptr)
	{
		return false;
	}
	m_EnvironmentTexturePath = filePath;
	return true;
}

bool EnvironmentMeshComponent::ReadIBLDiffuseTextureFromDDS(std::wstring filePath)
{
	m_IBLDiffuseTextureResource = ResourceManager::Instance->CreateTextureResource(filePath);
	assert(m_IBLDiffuseTextureResource != nullptr);
	if (m_IBLDiffuseTextureResource == nullptr)
	{
		return false;
	}
	m_IBLDiffuseTexturePath = filePath;
	return true;
}

bool EnvironmentMeshComponent::ReadIBLSpecularTextureFromDDS(std::wstring filePath)
{
	m_IBLSpecularTextureResource = ResourceManager::Instance->CreateTextureResource(filePath);
	assert(m_IBLSpecularTextureResource != nullptr);
	if (m_IBLSpecularTextureResource == nullptr)
	{
		return false;
	}
	m_IBLSpecularTexturePath = filePath;
	return true;
}

bool EnvironmentMeshComponent::ReadIBLBRDFTextureFromDDS(std::wstring filePath)
{
	m_IBLBRDFTextureResource = ResourceManager::Instance->CreateTextureResource(filePath);
	assert(m_IBLBRDFTextureResource != nullptr);
	if (m_IBLBRDFTextureResource == nullptr)
	{
		return false;
	}
	m_IBLBRDFTexturePath = filePath;
	return true;
}
