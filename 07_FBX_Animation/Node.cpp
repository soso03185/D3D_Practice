#include "Node.h"
#include "Model.h"
#include "Animation.h"
#include "TutorialApp.h"
#include "../Common/Helper.h"

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp/scene.h>

namespace Math = DirectX::SimpleMath;

void Node::Create(Model* model, aiNode* node, Animation* anim)
{
	m_Local = Math::Matrix(&node->mTransformation.a1).Transpose();
	m_Nname = node->mName.C_Str();
	
	m_MeshIndices.resize(node->mNumMeshes);
	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{
		UINT meshIndex = node->mMeshes[i];
		m_MeshIndices[i] = meshIndex;

		// Mesh와 Node의 WorldMatrix를 연결한다.
		model->m_Meshes[meshIndex].m_pNodeWorld = &m_World;
	}

	for (auto& child : anim->m_NodeAnims)
	{
		if (child.m_NodeName.compare(m_Nname) == 0) // equal
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

		// Todo ProgressTime 제대로 넣어주기 , 보간 작업
		m_pNodeAnimation->Evaluate(deltaTime, position, rotation, scaling);

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

void Node::SetTransform(const Vector3& val)
{
	m_Local *= Math::Matrix::CreateTranslation(val);
}
