#pragma once

#include "../Common/Helper.h"
#include <assimp\scene.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;


struct BoneInfo
{
	Matrix RelativeTransform;			//�θ�κ����� ������� ��ȯ
	Matrix OffsetMatrix;				// �������� �޽��� ������� ��ġ(��ȯ)
	std::string Name;						// ���̸�
	int NumChildren = 0;						// �ڽ��� ��
	int ParentBoneIndex = -1;
	std::vector<std::string> MeshNames;		// ���� ����� �޽����� �̸�
	BoneInfo() {}
	void Set(const aiNode* pNode)
	{
		Name = std::string(pNode->mName.C_Str());
		RelativeTransform = Matrix(&pNode->mTransformation.a1).Transpose();
		NumChildren = pNode->mNumChildren;
	}
	~BoneInfo()
	{

	}
};

class SkeletonResource
{
};

