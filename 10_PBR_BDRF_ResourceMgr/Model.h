#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Node.h"
#include "Animation.h"

struct aiScene;


// 그냥 mesh 랑 material , anim  만들어서 갖고 있는 애임.
class Model
{
public:
	Model();
	~Model();

	Node m_RootNode;
	vector<Mesh> m_Meshes;
	vector<Material> m_Materials;
	Animation m_Animation;

public:
	void Update(float deltaTime);
	bool ReadFile(std::string filePath);
	Material* GetMeshMaterial(UINT index);
};

