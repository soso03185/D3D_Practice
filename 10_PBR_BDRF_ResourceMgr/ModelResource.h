#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Node.h"
#include "Animation.h"

struct aiScene;

enum class ModelType
{
	STATIC,
	SKELETAL
};

// 그냥 mesh 랑 material , anim  만들어서 갖고 있는 애임.
class ModelResource
{
public:
	ModelResource();
	~ModelResource();

	Node m_RootNode;
	vector<Mesh> m_Meshes;
	vector<Material> m_Materials;
	Animation m_Animation;

public:
	void Update(float deltaTime);
	bool ReadFile(std::string filePath, ModelType modelType);
	Material* GetMeshMaterial(UINT index);
};

