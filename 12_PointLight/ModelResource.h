#pragma once

#include "NodeAnimation.h"
#include "Mesh.h"
#include "Material.h"
#include "Node.h"
#include "SkeletonResource.h"

struct aiScene;

// 그냥 mesh 랑 material , anim  만들어서 갖고 있는 애임.
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

