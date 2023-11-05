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
	Model() {}
	~Model() {}

	Node m_RootNode;
	vector<Mesh> m_Meshes;
	vector<Material> m_Materials;
	vector<Animation> m_Anims;

public:
	void Update() {}
	bool ReadFile(ID3D11Device* device, const char* filePath) {}

};

