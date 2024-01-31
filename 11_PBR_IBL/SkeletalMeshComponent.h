#pragma once
#include "SkeletalMeshInstance.h"
#include "Transform.h"
#include "Node.h"
#include "Animator.h"

#include <vector>
#include <list> 

class Material;
class ModelResource;
class SkeletonResource;

class SkeletalMeshComponent : public Transform
{
public:
	SkeletalMeshComponent();
	~SkeletalMeshComponent();

public:
	std::shared_ptr<ModelResource>  m_ModelResource;
	std::shared_ptr<Animator> m_pAnimator;

	std::vector<SkeletalMeshInstance> m_MeshInstances;
	std::list<SkeletalMeshComponent*>::iterator m_iterator;
	std::string m_SceneFilePath; // BeginPlay에서 로딩
	Node m_RootNode;

	bool ReadSceneResourceFromFBX(std::string filePath);
	void SetSceneResource(std::shared_ptr<ModelResource> val);
	void CreateHierachy(SkeletonResource* skeleton);

	Material* GetMaterial(UINT index);

	virtual void Update(float DeltaTime) override;
	virtual void OnBeginPlay();
	virtual void OnEndPlay();	

};