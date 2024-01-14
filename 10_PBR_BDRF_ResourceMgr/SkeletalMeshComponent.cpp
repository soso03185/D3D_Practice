#include "ResourceManager.h"
#include "D3DRenderManager.h"
#include "SkeletalMeshComponent.h"
#include "SkeletalMeshInstance.h"
#include "ModelResource.h"

SkeletalMeshComponent::SkeletalMeshComponent()
{
	D3DRenderManager::Instance->m_SkeletalMeshComponents.push_back(this);
	m_iterator = --D3DRenderManager::Instance->m_SkeletalMeshComponents.end();
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
	D3DRenderManager::Instance->m_SkeletalMeshComponents.erase(m_iterator);
}

void SkeletalMeshComponent::Update(float DeltaTime)
{
	__super::Update(DeltaTime);
	m_ModelResource->m_RootNode.Update(DeltaTime);
}

bool SkeletalMeshComponent::ReadSceneResourceFromFBX(std::string filePath)
{
	 //FBX 파일 읽기
	std::shared_ptr<ModelResource> sceneResource = ResourceManager::Instance->CreateModelResource(filePath, ModelType::SKELETAL);
	if (sceneResource == nullptr)
	{
		return false;
	}
	SetSceneResource(sceneResource);
	return true;
}

void SkeletalMeshComponent::SetSceneResource(std::shared_ptr<ModelResource> val)
{
	assert(val);
	m_ModelResource = val;
	// 인스턴스 생성
	m_MeshInstances.resize(m_ModelResource->m_Meshes.size());
	for (UINT i = 0; i < m_ModelResource->m_Meshes.size(); i++)
	{
		m_MeshInstances[i].Create(
			&m_ModelResource->m_Meshes[i], // mesh resource
			//&m_ModelResource->m_RootNode,
			&m_RootNode,
 			m_ModelResource->GetMeshMaterial(i)
			); 
	}

	//	m_BoundingBox.Center = Math::Vector3(m_ModelResource->m_AABBmin + m_ModelResource->m_AABBmax) * 0.5f;	// Calculate extent
	//	m_BoundingBox.Extents = Math::Vector3(m_ModelResource->m_AABBmax - m_ModelResource->m_AABBmin);	// Calculate extent
}

Material* SkeletalMeshComponent::GetMaterial(UINT index)
{
	assert(index < m_ModelResource->m_Materials.size());
	return &m_ModelResource->m_Materials[index];
}


void SkeletalMeshComponent::OnBeginPlay()
{
	if (!m_SceneFilePath.empty())
		ReadSceneResourceFromFBX(m_SceneFilePath);
}

void SkeletalMeshComponent::OnEndPlay()
{
}

