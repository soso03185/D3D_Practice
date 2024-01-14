#pragma once
#include "SkeletalMeshInstance.h"
#include "Transform.h"
#include "Node.h"

#include <vector>
#include <list> 

class Material;
class ModelResource;

class SkeletalMeshComponent : public Transform
{
public:
	SkeletalMeshComponent();
	~SkeletalMeshComponent();

	Node m_RootNode;

	std::string m_SceneFilePath; // BeginPlay에서 로딩
	std::shared_ptr<ModelResource>  m_ModelResource;
	std::vector<SkeletalMeshInstance> m_MeshInstances;

	std::list<SkeletalMeshComponent*>::iterator m_iterator;

	bool ReadSceneResourceFromFBX(std::string filePath);
	void SetSceneResource(std::shared_ptr<ModelResource> val);

	Material* GetMaterial(UINT index);

	virtual void Update(float DeltaTime) override;
	virtual void OnBeginPlay();
	virtual void OnEndPlay();
};