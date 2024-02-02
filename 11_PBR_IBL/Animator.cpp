#include "Animator.h"
#include "ModelResource.h"
#include "Node.h"

#include <cassert>
#include <cmath>
#include "ResourceManager.h"
 
void Animator::Binding(std::shared_ptr<ModelResource> modelResource, Node* rootNode)
{
	m_pModelResource = modelResource;
	m_pRootNode = rootNode;
}

void Animator::Update(float deltatime)
{
	m_AnimationProgressTime += deltatime;

	// for DurationTime deffered change 
	if (m_animChange == false)
	{
		m_AnimationProgressTime = (float)fmod(m_AnimationProgressTime, m_DurationTime);
	}
	else if (m_animChange == true)
	{
		if (m_AnimationProgressTime >= m_DurationTime)
		{
			m_DurationTime = m_pModelResource->m_Animations[m_AnimationIndex]->m_Duration;
			m_animChange = false;
		}
	}
}

void Animator::UpdateReference(int index)
{
	assert(index < m_pModelResource->m_Animations.size());
	auto animation = m_pModelResource->m_Animations[index];
	for (size_t i = 0; i < animation->m_NodeAnims.size(); i++)
	{
		NodeAnimation& nodeAnimation = animation->m_NodeAnims[i];
		Node* pBone = m_pRootNode->FindNode(nodeAnimation.m_NodeName);		
		assert(pBone != nullptr);
		pBone->m_pCurNodeAnimation = &animation->m_NodeAnims[i];
	}

	m_DurationTime = m_pModelResource->m_Animations[m_AnimationIndex]->m_Duration;
}

void Animator::SetAnimation(int index)
{
	assert(index < m_pModelResource->m_Animations.size());
	m_AnimationIndex = index;
	m_AnimationProgressTime = 0.0f;
	UpdateReference(index);
}

bool Animator::AddSceneAnimationFromFBX(std::string filePath)
{
	assert(m_pModelResource);
	auto animation = ResourceManager::Instance->CreateAnimationResource(filePath);
	if (!animation) {
		return false;
	}

	m_AnimationFilePath.push_back(filePath);
	m_pModelResource->m_Animations.push_back(animation);
	return true;
}
