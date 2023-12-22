// #include "pch.h"
#include "SkeletalMeshComponent.h"
#include "SceneComponent.h"
#include "../Common/Helper.h"
#include "ResourceManager.h"
// #include "TimeSystem.h"
#include "SkeletonResource.h"
#include "SkeletalMeshResource.h"
#include "SkeletalMeshInstance.h"
#include "Animation.h"



SkeletalMeshComponent::SkeletalMeshComponent(Actor* pOwner, const std::string& Name)
	:SceneComponent(pOwner, Name)
{
}


SkeletalMeshComponent::~SkeletalMeshComponent()
{
}




void SkeletalMeshModel::SetSceneResource(std::shared_ptr<SkeletalMeshSceneResource> val)
{
	m_SceneResource = val;

	// ���ҽ��� �ν��Ͻ� ó���Ѵ�.
	CreateHierachy(&m_SceneResource->m_Skeleton);	//�������� ����	

	m_MeshInstances.resize(m_SceneResource->m_SkeletalMeshResources.size());
	for (UINT i = 0; i < m_SceneResource->m_SkeletalMeshResources.size(); i++)
	{
		m_MeshInstances[i].Create(&m_SceneResource->m_SkeletalMeshResources[i], // mesh resource
			&m_SceneResource->m_Skeleton,	 // skeleton resource
			this,	// root node
			m_SceneResource->GetMeshMaterial(i));		//material resource 
	}
	UpdateNodeAnimationReference(0);	// �� ����� �ִϸ��̼� �������� ����	
}

bool SkeletalMeshModel::ReadSceneResourceFromFBX(std::string filePath)
{
	std::filesystem::path path = ToWString(string(filePath));
	LOG_MESSAGEA("Loading file: %s", filePath.c_str());

	GameTimer timer;
	timer.Tick();
	// ���ҽ� �Ŵ������� �����´�.
	SetSceneResource(ResourceManager::Instance->CreateSkeletalMeshSceneResource(filePath));
	if (!m_SceneResource) {
		return false;
	}


	timer.Tick();
	LOG_MESSAGEA("Complete file: %s %f", filePath.c_str(), timer.DeltaTime());
	return true;
}



Material* SkeletalMeshModel::GetMaterial(UINT index)
{
	assert(index < m_SceneResource->m_Materials.size());
	return &m_SceneResource->m_Materials[index];
}
void SkeletalMeshModel::Update(float deltaTime)
{
	if (!m_SceneResource->m_Animations.empty())
	{
		m_AnimationProressTime += deltaTime;
		m_AnimationProressTime = (float)fmod(m_AnimationProressTime, m_SceneResource->m_Animations[m_AnimationIndex]->Duration);

	}
	__super::Update(deltaTime);
}

void SkeletalMeshModel::UpdateNodeAnimationReference(UINT index)
{
	assert(index < m_SceneResource->m_Animations.size());
	auto animation = m_SceneResource->m_Animations[index];
	for (size_t i = 0; i < animation->NodeAnimations.size(); i++)
	{
		NodeAnimation& nodeAnimation = animation->NodeAnimations[i];
		Node* pNode = FindNode(nodeAnimation.NodeName);
		assert(pNode != nullptr);
		pNode->m_pNodeAnimation = &animation->NodeAnimations[i];
	}
}

void SkeletalMeshModel::SetWorldTransform(const Math::Matrix& transform)
{
	m_Local = transform;
}

void SkeletalMeshModel::PlayAnimation(UINT index)
{
	assert(index < m_SceneResource->m_Animations.size());
	m_AnimationIndex = index;
	m_AnimationProressTime = 0.0f;
	UpdateNodeAnimationReference(index);
}

void SkeletalMeshModel::CreateHierachy(SkeletonResource* skeleton)
{
	UINT count = skeleton->GetBoneCount();

	BoneInfo* pRootBone = skeleton->GetBone(0);
	m_Name = pRootBone->Name;
	m_Children.reserve(pRootBone->NumChildren);

	// 0�� ��Ʈ�� �����̳��̹Ƿ� ���� Node�� ���� �׷��Ƿ� 1������ �����Ѵ�.
	for (UINT i = 1; i < count; i++)
	{
		BoneInfo* pBone = skeleton->GetBone(i);
		assert(pBone != nullptr);
		assert(pBone->ParentBoneIndex != -1);

		Node* pParentNode = FindNode(skeleton->GetBoneName(pBone->ParentBoneIndex));
		assert(pParentNode != nullptr);

		auto& node = pParentNode->CreateChild();
		node.m_Name = pBone->Name;
		node.m_Local = pBone->RelativeTransform;
		node.m_Children.reserve(pBone->NumChildren);
		node.m_pParent = pParentNode;
		node.m_pAnimationTime = &m_AnimationProressTime;
	}
}