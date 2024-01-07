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

// �׳� ���⸸ �ϴ� �ſ��� Static Mesh Model ���� ������ ���� 
void ModelResource::Update(float deltaTime)
{
	m_RootNode.Update(deltaTime);
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

	const aiScene* m_fbxModel = importer.ReadFile(filePath, importFlags);

	if (!m_fbxModel)
	{
		LOG_ERRORA("Error loading FBX file: %s", importer.GetErrorString());
		return false;
	}

	m_Meshes.resize(m_fbxModel->mNumMeshes);
	m_Materials.resize(m_fbxModel->mNumMaterials);

	// fbx�� ����� ��(�ؽ���) ���� �������� ����.
	for (int i = 0; i < m_fbxModel->mNumMaterials; i++)
	{
		m_Materials[i].Create(m_fbxModel->mMaterials[i]);
	}

	// �� �ִϸ��̼� ���Ϳ� �ִϸ��̼� Ŭ�� �����ϰ� �� �ȿ��� ������ ���ε�
	// ToDo � �ִϸ��̼��� �÷��� �ϴ��� ���ؾ���.
	for (int i = 0; i < m_fbxModel->mNumAnimations; i++)
	{
		if (m_fbxModel->mAnimations[i] != nullptr)
			m_Animation.Create(m_fbxModel->mAnimations[i]);
	}

	// vertex , index ���� ���ε�
	for (int i = 0; i < m_fbxModel->mNumMeshes; i++)
	{
		if (modelType == ModelType::STATIC)
			m_Meshes[i].Create(m_fbxModel->mMeshes[i]);
		else if (modelType == ModelType::SKELETAL)
			m_Meshes[i].CreateBoneWeight(m_fbxModel->mMeshes[i]);
	}

	// ��� ��ȸ �ϸ鼭 ���ε�
	m_RootNode.Create(this, m_fbxModel->mRootNode, &m_Animation, &m_Nodes);

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

