#include "Node.h"
#include "TutorialApp.h"
#include "Model.h"
#include "../Common/Helper.h"

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp/scene.h>

void Node::Create(Model* model, aiNode* node)
{		
	m_Local = DirectX::SimpleMath::Matrix(&node->mTransformation.a1).Transpose();
	m_Nname = node->mName.C_Str();
	
	m_MeshIndices.resize(node->mNumMeshes);
	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{
		UINT meshIndex = node->mMeshes[i];
		m_MeshIndices[i] = meshIndex;

		// Mesh와 Node의 WorldMatrix를 연결한다.
		model->m_Meshes[meshIndex].m_pNodeWorld = &m_World;
	}

	if (m_pParent != nullptr)
		m_World = m_Local * m_pParent->m_World;
	else
		m_World = m_Local;

	m_Children.resize(node->mNumChildren);
	for (UINT i = 0; i < node->mNumChildren; ++i)
	{
		m_Children[i].m_pParent = this;
		m_Children[i].Create(model, node->mChildren[i]);
	}
}
