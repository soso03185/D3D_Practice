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

bool ModelResource::ReadFile(std::string filePath, ModelType modelType)
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

	if (modelType == ModelType::SKELETAL)
		m_Skeleton.Create(fbxModel);

	m_Meshes.resize(fbxModel->mNumMeshes);
	m_Materials.resize(fbxModel->mNumMaterials);

	// fbx�� ����� ��(�ؽ���) ���� �������� ����.
	for (int i = 0; i < fbxModel->mNumMaterials; i++)
	{
		m_Materials[i].Create(fbxModel->mMaterials[i]);
	}

	// vertex , index ���� ���ε�
	for (int i = 0; i < fbxModel->mNumMeshes; i++)
	{
		if (modelType == ModelType::STATIC)
			m_Meshes[i].Create(fbxModel->mMeshes[i]);
		else if (modelType == ModelType::SKELETAL)
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

