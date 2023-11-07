#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Node.h"

struct AnimationKey
{
	double Time;
	float Position[3];
	float Rotation[3];
	float Scaling;
};

struct NodeAnimation
{
	AnimationKey AnimationKeys;
public:
	void Evaluate() { }
};

struct Animation
{
	NodeAnimation NodeAnim;
	double Duration;
};


class Model
{
public:
	Model();
	~Model();

	Node m_RootNode;
	vector<Mesh> m_Meshes;
	vector<Material> m_Materials;
	vector<Animation> m_Anims;

	bool isDiffuse = true;
	bool isNormalMap = true;
	bool isSpecularMap = true;
	bool isEmissive = true;
	bool isOpacity = true;

public:
	void Update(float deltaTime);
	bool ReadFile(ID3D11Device* device, const char* filePath);
	Material* GetMaterial(UINT index);

	void Create();
};

