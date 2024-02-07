#include "ResourceManager.h"
#include "D3DRenderManager.h"
#include "SkeletalMeshComponent.h"
#include "SkeletalMeshInstance.h"
#include "ModelResource.h"

SkeletalMeshComponent::SkeletalMeshComponent()
{
	D3DRenderManager::Instance->m_SkeletalMeshComponents.push_back(this);
	m_iterator = --D3DRenderManager::Instance->m_SkeletalMeshComponents.end();

	m_pAnimator = make_shared<Animator>();
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
	D3DRenderManager::Instance->m_SkeletalMeshComponents.erase(m_iterator);
}

void SkeletalMeshComponent::Update(float deltaTime)
{
	__super::Update(deltaTime);

	if (!m_ModelResource->m_Animations.empty())
	{
		m_pAnimator->Update(deltaTime);
	}

	m_RootNode.Update(deltaTime);
}

bool SkeletalMeshComponent::ReadSceneResourceFromFBX(std::string filePath)
{
	 //FBX 파일 읽기
	std::shared_ptr<ModelResource> sceneResource = ResourceManager::Instance->CreateSkeletalModelResource(filePath);
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

	CreateHierachy(&m_ModelResource->m_Skeleton);

	// 인스턴스 생성
	m_MeshInstances.resize(m_ModelResource->m_Meshes.size());
	for (UINT i = 0; i < m_ModelResource->m_Meshes.size(); i++)
	{
		Mesh* meshResource = &m_ModelResource->m_Meshes[i];
		Material* material = nullptr;

		if (m_ModelResource->m_Materials.size() > i) 
		{
			material = &m_ModelResource->m_Materials[i];
		}

		m_MeshInstances[i].Create(meshResource, &m_RootNode, material); 
	}	

	m_pAnimator->Binding(m_ModelResource, &m_RootNode);
	m_pAnimator->UpdateReference(0);	// 각 노드의 애니메이션 정보참조 연결		
}

void SkeletalMeshComponent::CreateHierachy(SkeletonResource* skeleton)
{
	UINT count = skeleton->GetBoneCount();

	BoneInfo* pRootBone = skeleton->GetBone(0);
	m_RootNode.m_Name = pRootBone->Name;
	m_RootNode.m_Children.reserve(pRootBone->NumChildren);
	m_RootNode.SetParent(this);

	// 0번 루트는 컨테이너이므로 현재 Node와 같다 그러므로 1번부터 시작한다.
	for (UINT i = 1; i < count; i++)
	{
		BoneInfo* pBoneInfo = skeleton->GetBone(i);
		assert(pBoneInfo != nullptr);
		assert(pBoneInfo->ParentBoneIndex != -1);

		Node* pParentBone = m_RootNode.FindNode(skeleton->GetBoneName(pBoneInfo->ParentBoneIndex));
		assert(pParentBone != nullptr);

		auto& ChildBone = pParentBone->CreateChild();
		ChildBone.m_Name = pBoneInfo->Name;
		ChildBone.m_Local = pBoneInfo->RelativeTransform;
		ChildBone.m_Children.reserve(pBoneInfo->NumChildren);
		ChildBone.m_pParent = pParentBone;
		ChildBone.m_pAnimationTime = &m_pAnimator->m_AnimationProgressTime;
	}
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
