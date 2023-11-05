#pragma once
#include "../Common/Helper.h"
#include "Mesh.h"
#include "Material.h"

#include <string>

struct Model;
struct NodeAnimation;
struct aiNode;

using namespace DirectX::SimpleMath;
using namespace DirectX;

class Node
{
public:
	Matrix m_Local;
	Matrix m_World;
	std::string m_Nname;
	Node* m_pParent;
	NodeAnimation* m_NodeAnimPtr;

	vector<Mesh> m_MeshIndices;
	vector<Node> m_Materials;

public:
	void Update() {};	
	void Create(Model* model , aiNode* node);
};

