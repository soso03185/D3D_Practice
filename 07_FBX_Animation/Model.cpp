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
	unsigned int importFlags = aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded;

	m_fbxModel = importer.ReadFile("../Resource/BoxHuman.fbx", importFlags);

	if (!m_fbxModel)
	{
		LOG_ERRORA("Error loading FBX file: %s", importer.GetErrorString());
		return false;
	}

	m_Meshes.resize(m_fbxModel->mNumMeshes);
	m_Materials.resize(m_fbxModel->mNumMaterials);

	// fbx에 적용된 맵(텍스쳐) 들을 가져오는 과정.
	for (int i = 0; i < m_fbxModel->mNumMaterials; i++)
	{
		m_Materials[i].Create(device, m_fbxModel->mMaterials[i]);
	}

	// 각 애니메이션 벡터에 애니메이션 클립 생성하고 그 안에서 정보들 바인딩
	// ToDo 어떤 애니메이션을 플레이 하는지 비교해야함.
	for (int i = 0; i < m_fbxModel->mNumAnimations; i++)
	{
		if (m_fbxModel->mAnimations[i] != nullptr)
			m_Animation.Create(m_fbxModel->mAnimations[i]);
	}

	// 노드 순회 하면서 바인딩
	m_RootNode.Create(this, m_fbxModel->mRootNode, &m_Animation);

	// vertex , index 정보 바인딩
	for (int i = 0; i < m_fbxModel->mNumMeshes; i++)
	{
		m_Meshes[i].Create(device, m_fbxModel->mMeshes[i]);
	}

	return true;
}

Material* Model::GetMaterial(UINT index)
{
	return &m_Materials[index];
}