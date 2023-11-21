#pragma once
#include "../Common/Helper.h"
#include "Mesh.h"
#include "Material.h"

#include <string>

class Animation;
struct Model;
struct NodeAnimation;
struct aiNode;
struct aiScene;

using namespace DirectX::SimpleMath;
using namespace DirectX;

class Node
{
public:
	Matrix m_Local;
	Matrix m_World;
	std::string m_Name;

	NodeAnimation* m_pNodeAnimation;
	Node* m_pParent;

	vector<UINT> m_MeshIndices;
	vector<Node> m_Children;	

public:
	void Create(Model* model , aiNode* node, Animation* anim);
	void Update(float deltaTime);
};