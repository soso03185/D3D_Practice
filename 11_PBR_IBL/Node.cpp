#include "Node.h"
#include "ModelResource.h"
#include "NodeAnimation.h"
#include "TutorialApp.h"
#include "../Common/Helper.h"

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp/scene.h>

namespace Math = DirectX::SimpleMath;

void Node::Update(float deltaTime)
{
	if (m_pCurNodeAnimation != nullptr)
	{
		Math::Matrix transform = m_pCurNodeAnimation->Evaluate(*m_pAnimationTime);
		m_Local = transform;

		// 애니메이션 전환될 상황인 경우
		if (m_pNextNodeAnimation != nullptr)
		{
			m_AnimChangeTime += deltaTime;
			float changingAnimTime = 1.f;

			// 전환 될 애니메이션의 키 값을 Matrix로 가져오기
			Math::Matrix nextTransform = m_pNextNodeAnimation->Evaluate(*m_pAnimationTime);
			m_Local = Math::Matrix::Lerp(transform, nextTransform, m_AnimChangeTime);

			if (m_AnimChangeTime >= changingAnimTime)
			{
				m_pCurNodeAnimation = m_pNextNodeAnimation;
				m_pNextNodeAnimation = nullptr;
				m_AnimChangeTime = 0;
			}
		}
	}

	if (m_pParent != nullptr) 
	{
		m_World = m_Local * m_pParent->m_World;
	}
	else
	{
		m_World = m_Local;
	}


	for (auto& child : m_Children)
	{
		child.Update(deltaTime);
	}
}

Node* Node::FindNode(const std::string& name)
{
	if (m_Name == name)
		return this;

	for (auto& child : m_Children)
	{
		Node* found = child.FindNode(name);
		if (found != nullptr)
			return found;
	}

	return nullptr;
}

Node& Node::CreateChild()
{
	return m_Children.emplace_back();
}

