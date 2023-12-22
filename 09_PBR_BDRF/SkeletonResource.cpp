
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "SkeletonResource.h"
#include "Node.h"



void SkeletonResource::Create(const aiScene* pScene)
{
	Name = pScene->mName.C_Str();
	int NumNode = 0;
	CountNode(NumNode, pScene->mRootNode);
	Bones.reserve(NumNode);	// ���� �Ź� ���Ҵ�����ʵ��� ������ Ȯ��. �߰��Ҷ����� �ε����� �����ǹǷ� ũ��� �������� �ʴ´�.
	AddBone(pScene, pScene->mRootNode);

	//���� �� ������ �Ŀ� OffsetMatrix����
	for (UINT i = 0; i < pScene->mNumMeshes; i++)
	{
		aiMesh* pMesh = pScene->mMeshes[i];
		if (!pMesh->HasBones())
			continue;

		for (UINT iBone = 0; iBone < pMesh->mNumBones; iBone++)
		{
			aiBone* pAiBone = pMesh->mBones[iBone];
			BoneInfo* pBone = FindBone(pAiBone->mName.C_Str());
			pBone->OffsetMatrix = Math::Matrix(&pAiBone->mOffsetMatrix.a1).Transpose();
		}
	}
}

BoneInfo* SkeletonResource::AddBone(const aiScene* pScene, const aiNode* pNode)
{
	BoneInfo& bone = Bones.emplace_back();
	bone.Set(pNode);

	int BoneIndex = (int)(Bones.size() - 1);
	BoneMappingTable[bone.Name] = BoneIndex;
	UINT numMesh = pNode->mNumMeshes;
	if (numMesh > 0)
	{
		bone.MeshNames.resize(numMesh);
		for (UINT i = 0; i < numMesh; ++i)
		{
			UINT meshIndex = pNode->mMeshes[i];
			std::string meshName = pScene->mMeshes[meshIndex]->mName.C_Str();
			bone.MeshNames[i] = meshName;
			MeshMappingTable[meshName] = BoneIndex;
		}
	}

	UINT numChild = pNode->mNumChildren;
	for (UINT i = 0; i < numChild; ++i)
	{
		BoneInfo* child = AddBone(pScene, pNode->mChildren[i]);
		child->ParentBoneIndex = BoneIndex;
	}
	return &Bones[BoneIndex];
}


BoneInfo* SkeletonResource::FindBone(const std::string& name)
{
	auto iter = BoneMappingTable.find(name);
	if (iter == BoneMappingTable.end())
		return nullptr;
	return &Bones[iter->second];
}

BoneInfo* SkeletonResource::GetBone(int index)
{
	if (index < 0 || index >= Bones.size())
		return nullptr;
	return &Bones[index];
}

int SkeletonResource::GetBoneIndexByBoneName(const std::string& boneName)
{
	auto iter = BoneMappingTable.find(boneName);
	if (iter == BoneMappingTable.end())
		return -1;
	return iter->second;
}

int SkeletonResource::GetBoneIndexByMeshName(const std::string& meshName)
{
	auto iter = MeshMappingTable.find(meshName);
	if (iter == MeshMappingTable.end())
		return -1;
	return iter->second;
}

void SkeletonResource::CountNode(int& Count, const aiNode* pNode)
{
	Count++;
	std::string name = pNode->mName.C_Str();
	BoneMappingTable[name] = Count;

	UINT numChild = pNode->mNumChildren;
	for (UINT i = 0; i < numChild; ++i)
	{
		CountNode(Count, pNode->mChildren[i]);
	}
}
