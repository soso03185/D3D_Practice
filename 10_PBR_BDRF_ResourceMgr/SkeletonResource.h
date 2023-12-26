#pragma once

#include "../Common/Helper.h"
#include <assimp\scene.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;


struct BoneInfo
{
	Matrix RelativeTransform;			//부모로부터의 상대적인 변환
	Matrix OffsetMatrix;				// 본에서의 메쉬의 상대적인 위치(변환)
	std::string Name;						// 본이름
	int NumChildren = 0;						// 자식의 수
	int ParentBoneIndex = -1;
	std::vector<std::string> MeshNames;		// 본에 연결된 메쉬들의 이름
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

