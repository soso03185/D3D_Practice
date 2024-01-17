#pragma once
#include <assimp/scene.h>
#include <directxtk\SimpleMath.h>
#include <map>

#include "..\Common\Helper.h"

using namespace DirectX::SimpleMath;
namespace Math = DirectX::SimpleMath;

struct BoneInfo
{
	Math::Matrix RelativeTransform;			//�θ�κ����� ������� ��ȯ
	Math::Matrix OffsetMatrix;				// �������� �޽��� ������� ��ġ(��ȯ)
	std::string Name;						// ���̸�
	int NumChildren = 0;						// �ڽ��� ��
	int ParentBoneIndex = -1;
	std::vector<std::string> MeshNames;		// ���� ����� �޽����� �̸�
	BoneInfo() {}
	void Set(const aiNode* pNode)
	{
		Name = std::string(pNode->mName.C_Str());
		RelativeTransform = Math::Matrix(&pNode->mTransformation.a1).Transpose();
		NumChildren = pNode->mNumChildren;
	}
	~BoneInfo()
	{

	}
};

// �� ������ �����ϴ� ����ü
struct SkeletonResource
{
public:
	std::string Name;
	std::vector<BoneInfo> Bones;
	std::map<std::string, int> BoneMappingTable;
	std::map<std::string, int> MeshMappingTable;

	void Create(const aiScene* pScene);
	BoneInfo* AddBone(const aiScene* pScene, const aiNode* pNode);
	BoneInfo* FindBone(const std::string& name);
	BoneInfo* GetBone(int index);
	int GetBoneIndexByBoneName(const std::string& boneName);
	int GetBoneIndexByMeshName(const std::string& meshName);
	int GetBoneCount() {
		return (int)Bones.size();
	}
	const std::string& GetBoneName(int index) {
		return Bones[index].Name;
	}
	void CountNode(int& Count, const aiNode* pNode);
};

