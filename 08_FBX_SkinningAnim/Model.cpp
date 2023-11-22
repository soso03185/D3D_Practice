#include "Model.h"
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp/scene.h>

Model::Model()
{
}

Model::~Model()
{
	m_Meshes.clear();
	m_Materials.clear();
}

void Model::Update(float deltaTime)
{
	m_RootNode.Update(deltaTime);
}

bool Model::ReadFile(ID3D11Device* device, const char* filePath)
{
	Assimp::Importer importer;
	unsigned int importFlags = aiProcess_Triangulate | // �ﰢ������ ��ȯ
		aiProcess_GenNormals |   // �븻 ����
		aiProcess_GenUVCoords |  // UV ����
		aiProcess_CalcTangentSpace |  // ź��Ʈ ����
		aiProcess_LimitBoneWeights |  // ���� ������ �޴� ������ �ִ� ������ 4���� ����
		aiProcess_ConvertToLeftHanded;   // �޼� ��ǥ��� ��ȯ

	m_fbxModel = importer.ReadFile(filePath, importFlags);

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
		m_Materials[i].Create(device, m_fbxModel->mMaterials[i]);
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
		m_Meshes[i].CreateBoneWeightVertex(device, m_fbxModel->mMeshes[i]);
	}

	// ��� ��ȸ �ϸ鼭 ���ε�
	m_RootNode.Create(this, m_fbxModel->mRootNode, &m_Animation);

	return true;
}

Material* Model::GetMaterial(UINT index)
{
	return &m_Materials[index];
}