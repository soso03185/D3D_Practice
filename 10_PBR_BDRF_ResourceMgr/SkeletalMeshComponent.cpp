#include "SkeletalMeshComponent.h"
#include "D3DRenderManager.h"
 
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
}

