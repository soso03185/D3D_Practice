#include "Node.h"
#include "ModelResource.h"
#include "NodeAnimation.h"
#include "TutorialApp.h"
#include "../Common/Helper.h"

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp/scene.h>

namespace Math = DirectX::SimpleMath;

void Node::Create(ModelResource* model, aiNode* node, Animation* anim)
{
	m_Local = Math::Matrix(&node->mTransformation.a1).Transpose();
	m_Name = node->mName.C_Str();

	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{
		UINT meshIndex = node->mMeshes[i];

		// Mesh와 Node의 WorldMatrix를 연결한다.
		 model->m_Meshes[meshIndex].m_pNodeWorld = &m_World; 
	}

	for (auto& mesh : model->m_Meshes)
	{
		for (auto& boneRef : mesh.m_BoneReferences)
		{
			if (boneRef.NodeName.compare(m_Name) == 0)
			{
				boneRef.NodeWorldMatrixPtr = &m_World;
			}
		}
	}

	for (auto& child : anim->m_NodeAnims)
	{
		if (child.m_NodeName.compare(m_Name) == 0) // if  equal
		{
			m_pNodeAnimation = &child;
			break;
		}
	}

	m_Children.resize(node->mNumChildren);
	for (UINT i = 0; i < node->mNumChildren; ++i)
	{
		m_Children[i].m_pParent = this;
		m_Children[i].Create(model, node->mChildren[i], anim);
	}
}

void Node::Update(float deltaTime)
{
	if (m_pNodeAnimation != nullptr)
	{
		Math::Vector3 position, scaling;
		Math::Quaternion rotation;

		m_pNodeAnimation->Evaluate(*m_pAnimationTime, position, rotation, scaling);

		m_Local = Math::Matrix::CreateScale(scaling) *
			Math::Matrix::CreateFromQuaternion(rotation) *
			Math::Matrix::CreateTranslation(position);
	}	

	if (m_pParent != nullptr)
		m_World = m_Local * m_pParent->m_World;
	else
		m_World = m_Local;

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

