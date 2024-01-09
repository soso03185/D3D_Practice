#pragma once
#include "../Common/Helper.h"
#include "Mesh.h"
#include "Material.h"

#include <string>

class Animation;
struct ModelResource;
struct NodeAnimation;
struct aiNode;

using namespace DirectX::SimpleMath;
using namespace DirectX;
namespace Math = DirectX::SimpleMath;

class Node
{
public:
	Matrix m_Local;
	Matrix m_World;
	std::string m_Name;

	vector<Node> m_Children;	
	NodeAnimation* m_pNodeAnimation = nullptr;
	Node* m_pParent = nullptr;


public:
	void Create(ModelResource* model , aiNode* node, Animation* anim);
	void Update(float deltaTime);

	Node* FindNode(const std::string& name);
};