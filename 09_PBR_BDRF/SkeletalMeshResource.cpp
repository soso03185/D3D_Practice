// #include "pch.h"
#include "../Common/Helper.h"

#include "SkeletalMeshResource.h"
#include "D3DRenderManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include <assimp/cimport.h>
#include "ResourceManager.h"
// #include "TimeSystem.h"


SkeletalMeshResource::SkeletalMeshResource()
{

}

SkeletalMeshResource::~SkeletalMeshResource()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

void SkeletalMeshResource::Create(aiMesh* mesh, SkeletonResource* skeleton)
{
	//Skeletal Mesh
	m_Vertices.resize(mesh->mNumVertices);
	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		m_Vertices[i].Position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		m_Vertices[i].Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		m_Vertices[i].TexCoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		m_Vertices[i].Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

	}
	UINT meshBoneCount = mesh->mNumBones;	// �޽��� ����� ������
	m_BoneReferences.resize(meshBoneCount); // �� ���� ���� �����̳� ũ�� ����
	for (UINT i = 0; i < meshBoneCount; ++i)
	{
		aiBone* pAiBone = mesh->mBones[i];

		// ���̷��濡�� �������� ã�´�.
		UINT boneIndex = skeleton->GetBoneIndexByBoneName(pAiBone->mName.C_Str());
		assert(boneIndex != -1);
		BoneInfo* pBone = skeleton->GetBone(boneIndex);
		assert(pBone != nullptr);

		m_BoneReferences[i].NodeName = pAiBone->mName.C_Str();
		m_BoneReferences[i].BoneIndex = boneIndex;
		m_BoneReferences[i].OffsetMatrix = Math::Matrix(&pAiBone->mOffsetMatrix.a1).Transpose();

		// ���� ����� ���ؽ����� ó��
		for (UINT j = 0; j < pAiBone->mNumWeights; ++j)
		{
			UINT vertexID = pAiBone->mWeights[j].mVertexId;
			float weight = pAiBone->mWeights[j].mWeight;
			m_Vertices[vertexID].AddBoneData(boneIndex, weight);
		}
	}
	CreateVertexBuffer(&m_Vertices[0], (UINT)m_Vertices.size());


	// �ε��� ���� ����
	m_Indices.resize(mesh->mNumFaces * 3);
	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		m_Indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
		m_Indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		m_Indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
	}
	CreateIndexBuffer(&m_Indices[0], (UINT)m_Indices.size());
}

void SkeletalMeshResource::CreateVertexBuffer(BoneWeightVertex* vertices, UINT vertexCount)
{
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(BoneWeightVertex) * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices;
	HR_T(D3DRenderManager::m_pDevice->CreateBuffer(&bd, &vbData, &m_pVertexBuffer));

	// ���ؽ� ���� ����
	m_VertexCount = vertexCount;
	m_VertexBufferStride = sizeof(BoneWeightVertex);
	m_VertexBufferOffset = 0;
}

void SkeletalMeshResource::CreateIndexBuffer(WORD* indices, UINT indexCount)
{
	// �ε��� ���� ����.
	m_IndexCount = indexCount;

	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(WORD) * indexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices;
	HR_T(D3DRenderManager::m_pDevice->CreateBuffer(&bd, &ibData, &m_pIndexBuffer));
}




bool SkeletalMeshSceneResource::Create(std::string filePath)
{
	std::filesystem::path path = ToWString(string(filePath));

	Assimp::Importer importer;

	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);	// $assimp_fbx$ ��� ��������


	unsigned int importFlags = aiProcess_Triangulate | // �ﰢ������ ��ȯ
		aiProcess_GenNormals |	// �븻 ����/
		aiProcess_GenUVCoords |		// UV ����
		aiProcess_CalcTangentSpace |  // ź��Ʈ ����
		aiProcess_LimitBoneWeights |	// ���� ������ �޴� ������ �ִ� ������ 4���� ����
		aiProcess_GenBoundingBoxes | // �ٿ�� �ڽ� ����
		aiProcess_ConvertToLeftHanded;	// �޼� ��ǥ��� ��ȯ

	const aiScene* scene = importer.ReadFile(filePath, importFlags);
	if (!scene) {
		LOG_ERRORA("Error loading file: %s", importer.GetErrorString());
		return false;
	}

	//������ Ȯ�ο� ���ӿ� �ʿ����
	aiMetadata* pAiMetaData = scene->mMetaData;
	std::vector<MetaData> metaDataList;
	if (pAiMetaData)
	{	// Iterate through the metadata
		for (unsigned int i = 0; i < pAiMetaData->mNumProperties; i++)
		{
			MetaData& data = metaDataList.emplace_back();
			data.Name = pAiMetaData->mKeys[i].C_Str();
			data.SetData(pAiMetaData->mValues[i]);
		}
	}

	m_Skeleton.Create(scene);

	m_Materials.resize(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		m_Materials[i].Create(scene->mMaterials[i]);
	}

	m_SkeletalMeshResources.resize(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		m_SkeletalMeshResources[i].Create(scene->mMeshes[i], &m_Skeleton);
	}

	m_AABBmin = Math::Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_AABBmax = Math::Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* pMesh = scene->mMeshes[i];
		Math::Vector3 meshMin = Math::Vector3(pMesh->mAABB.mMin.x, pMesh->mAABB.mMin.y, pMesh->mAABB.mMin.z);
		Math::Vector3 meshMax = Math::Vector3(pMesh->mAABB.mMax.x, pMesh->mAABB.mMax.y, pMesh->mAABB.mMax.z);

		m_AABBmin = Math::Vector3::Min(m_AABBmin, meshMin);
		m_AABBmax = Math::Vector3::Max(m_AABBmax, meshMax);
	}

	// SceneResource�� �⺻ �ִϸ��̼� �߰��Ѵ�.

	assert(scene->mNumAnimations < 2); // �ִϸ��̼��� ���ų� 1�������Ѵ�. 
	// ����� �ִϸ��̼��� �ϳ��� ��ġ�� ����� FBX export���� NLA��Ʈ��,��� �׼� �ɼ��� ���ϴ�.
	if (scene->HasAnimations())
	{
		const aiAnimation* pAiAnimation = scene->mAnimations[0];
		// ä�μ��� aiAnimation �ȿ��� �ִϸ��̼� ������  ǥ���ϴ� aiNode�� �����̴�.
		assert(pAiAnimation->mNumChannels > 1); // �ִϸ��̼��� �ִٸ� aiNode �� �ϳ� �̻� �־���Ѵ�.

		shared_ptr<Animation> anim = make_shared<Animation>();
		anim->Create(filePath, pAiAnimation);
		m_Animations.push_back(anim);
	}
	importer.FreeScene();
	return true;
}



void SkeletalMeshSceneResource::AddAnimation(std::shared_ptr<Animation> animation)
{
	m_Animations.push_back(animation);
}

Material* SkeletalMeshSceneResource::GetMeshMaterial(UINT index)
{
	assert(index < m_Materials.size());
	UINT mindex = m_SkeletalMeshResources[index].m_MaterialIndex;
	assert(mindex < m_Materials.size());
	return &m_Materials[mindex];
}