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

// �׳� mesh �� material , anim  ���� ���� �ִ� ����.
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

