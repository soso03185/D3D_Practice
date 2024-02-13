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

		// �ִϸ��̼� ��ȯ�� ��Ȳ�� ���
		if (m_pNextNodeAnimation != nullptr)
		{
			// ��ȯ �� �ִϸ��̼��� Ű ���� Matrix�� ��������
			Math::Matrix nextTransform = m_pNextNodeAnimation->Evaluate(*m_pAnimationTime);

			float changingAnimTime = 0.2f;
			m_AnimChangeTime += deltaTime;
			m_Local = Math::Matrix::Lerp(transform, nextTransform, m_AnimChangeTime *  (1 / changingAnimTime));

			if (m_AnimChangeTime >= changingAnimTime)
			{
				m_pCurNodeAnimation = m_pNextNodeAnimation;
				m_pNextNodeAnimation = nullptr;
				m_AnimChangeTime = 0;
			}
		}
	}
	else
	{

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
