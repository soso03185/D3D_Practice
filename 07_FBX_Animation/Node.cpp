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
	LOG_MESSAGE(m_Nname.C_str());

	if (node->mParent)
	{

	}

}
