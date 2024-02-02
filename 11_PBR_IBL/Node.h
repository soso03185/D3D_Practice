#pragma once
#include "../Common/Helper.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"

#include <string>

class Animation;
class NodeAnimation;
struct ModelResource;
struct aiNode;

namespace Math = DirectX::SimpleMath;

class Node : public Transform
{
public:
	vector<Node> m_Children;	
	NodeAnimation* m_pCurNodeAnimation = nullptr;
	NodeAnimation* m_pNextNodeAnimation = nullptr;
	float* m_pAnimationTime = nullptr;	// 현재 노드가 애니메이션에서 어느 시간에 있는지
	float m_AnimChangeTime = 0;

public:
	void Update(float deltaTime);
	void SetParent(Transform* pParent) { m_pParent = pParent; };

	Node* FindNode(const std::string& name);
	Node& CreateChild();
};