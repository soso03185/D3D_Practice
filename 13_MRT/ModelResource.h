#pragma once

#include "NodeAnimation.h"
#include "Mesh.h"
#include "Material.h"
#include "Node.h"
#include "SkeletonResource.h"

struct aiScene;

// �׳� mesh �� material , anim  ���� ���� �ִ� ����.
class ModelResource
{
public:
	ModelResource();
	~ModelResource();

	vector<Mesh> m_Meshes;
	vector<Material> m_Materials;
	SkeletonResource m_Skeleton;

	std::vector<std::shared_ptr<Animation>> m_Animations;

public:
	bool ReadStaticFile(std::string filePath);
	bool ReadSkeletalFile(std::string filePath);
	Material* GetMeshMaterial(UINT index);
};

