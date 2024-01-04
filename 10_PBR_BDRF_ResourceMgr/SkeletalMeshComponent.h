#pragma once
#include "SkeletalMeshInstance.h"
#include "Transform.h"

#include <vector>
#include <list> 

class SkeletalMeshComponent : public Transform
{
public:
	SkeletalMeshComponent();
	~SkeletalMeshComponent();

public:
	virtual void Update(float DeltaTime) override;


public:
	std::vector<SkeletalMeshInstance> m_MeshInstances;
	std::list<SkeletalMeshComponent*>::iterator m_iterator;
};

