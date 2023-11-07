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
	NodeAnimation* m_NodeAnimPtr;

	Node* m_pParent;

	vector<UINT> m_MeshIndices;
	vector<Node> m_Children;	

public:
	void Update() {};	
	void Create(Model* model , aiNode* node);
};