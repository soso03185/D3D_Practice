#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Node.h"
#include "Animation.h"

struct aiScene;


// �׳� mesh �� material , anim  ���� ���� �ִ� ����.
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

