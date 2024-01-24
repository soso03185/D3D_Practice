#include "ModelResource.h"
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp/scene.h>

ModelResource::ModelResource()
{
}

ModelResource::~ModelResource()
{
	m_Meshes.clear();
	m_Materials.clear();
}

bool ModelResource::ReadStaticFile(std::string filePath)
{
	Assimp::Importer importer;

	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

	unsigned int importFlags = aiProcess_Triangulate | // �ﰢ������ ��ȯ
		aiProcess_GenNormals |   // �븻 ����
		aiProcess_GenUVCoords |  // UV ����
		aiProcess_CalcTangentSpace |  // ź��Ʈ ����
		aiProcess_LimitBoneWeights |  // ���� ������ �޴� ������ �ִ� ������ 4���� ����
		aiProcess_PreTransformVertices |  // �������� ������ ���� �������� �޾ƿͼ� ����� ���·� ��
		aiProcess_ConvertToLeftHanded;   // �޼� ��ǥ��� ��ȯ

	const aiScene* fbxModel = importer.ReadFile(filePath, importFlags);

	if (!fbxModel)
	{
		LOG_ERRORA("Error loading FBX file: %s", importer.GetErrorString());
		return false;
	}

	// fbx�� ����� ��(�ؽ���) ���� �������� ����.
	m_Materials.resize(fbxModel->mNumMaterials);
	for (int i = 0; i < fbxModel->mNumMaterials; i++)
	{
		m_Materials[i].Create(fbxModel->mMaterials[i]);
	}

	// vertex , index ���� ���ε�
	m_Meshes.resize(fbxModel->mNumMeshes);
	for (int i = 0; i < fbxModel->mNumMeshes; i++)
	{
		m_Meshes[i].Create(fbxModel->mMeshes[i]);
	}

	importer.FreeScene();
	return true;
}

bool ModelResource::ReadSkeletalFile(std::string filePath)
{
	Assimp::Importer importer;

	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

	unsigned int importFlags = aiProcess_Triangulate | // �ﰢ������ ��ȯ
		aiProcess_GenNormals |   // �븻 ����
		aiProcess_GenUVCoords |  // UV ����
		aiProcess_CalcTangentSpace |  // ź��Ʈ ����
		aiProcess_LimitBoneWeights |  // ���� ������ �޴� ������ �ִ� ������ 4���� ����
		aiProcess_ConvertToLeftHanded;   // �޼� ��ǥ��� ��ȯ

	const aiScene* fbxModel = importer.ReadFile(filePath, importFlags);

	if (!fbxModel)
	{
		LOG_ERRORA("Error loading FBX file: %s", importer.GetErrorString());
		return false;
	}

	m_Skeleton.Create(fbxModel);

	// fbx�� ����� ��(�ؽ���) ���� �������� ����.
	m_Materials.resize(fbxModel->mNumMaterials);
	for (int i = 0; i < fbxModel->mNumMaterials; i++)
	{
		m_Materials[i].Create(fbxModel->mMaterials[i]);
	}

	// vertex , index ���� ���ε�
	m_Meshes.resize(fbxModel->mNumMeshes);
	for (int i = 0; i < fbxModel->mNumMeshes; i++)
	{
		m_Meshes[i].CreateBoneWeight(fbxModel->mMeshes[i], &m_Skeleton);
	}

	// SceneResource�� �⺻ �ִϸ��̼� �߰��Ѵ�.
	assert(fbxModel->mNumAnimations < 2); // �ִϸ��̼��� ���ų� 1�������Ѵ�. 
	// ����� �ִϸ��̼��� �ϳ��� ��ġ�� ����� FBX export���� NLA��Ʈ��,��� �׼� �ɼ��� ���ϴ�.
	if (fbxModel->HasAnimations())
	{
		const aiAnimation* pAiAnimation = fbxModel->mAnimations[0];
		// ä�μ��� aiAnimation �ȿ��� �ִϸ��̼� ������  ǥ���ϴ� aiNode�� �����̴�.
		assert(pAiAnimation->mNumChannels > 1); // �ִϸ��̼��� �ִٸ� aiNode �� �ϳ� �̻� �־���Ѵ�.

		shared_ptr<Animation> anim = make_shared<Animation>();
		anim->Create(filePath, pAiAnimation);
		m_Animations.push_back(anim);
	}

	importer.FreeScene();
	return true;
}

Material* ModelResource::GetMeshMaterial(UINT index)
{
	assert(index < m_Meshes.size());
	UINT mindex = m_Meshes[index].m_MaterialIndex;
	assert(mindex < m_Materials.size());
	return &m_Materials[mindex];
}

