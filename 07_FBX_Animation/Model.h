#pragma once

//#include "Mesh.h"
//#include "Material.h"
#include "Node.h"
#include "Animation.h"

class Mesh;
class Material;
struct aiScene;

class Model
{
public:
	Model();
	~Model();

	const aiScene* m_fbxModel;

	Node m_RootNode;
	vector<Mesh> m_Meshes;
	vector<Material> m_Materials;
	Animation m_Animation;

	bool isDiffuse = true;
	bool isNormalMap = true;
	bool isSpecularMap = true;
	bool isEmissive = true;
	bool isOpacity = true;

public:
	void Update(float deltaTime);
	bool ReadFile(ID3D11Device* device, const char* filePath);
	Material* GetMaterial(UINT index);
};

