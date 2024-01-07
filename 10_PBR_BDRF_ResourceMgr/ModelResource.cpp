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

// 그냥 띄우기만 하는 거에서 Static Mesh Model 에는 사용되지 않음 
void ModelResource::Update(float deltaTime)
{
	m_RootNode.Update(deltaTime);
}

bool ModelResource::ReadFile(std::string filePath, ModelType modelType)
{
	Assimp::Importer importer;

	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

	unsigned int importFlags = aiProcess_Triangulate | // 삼각형으로 변환
		aiProcess_GenNormals |   // 노말 생성
		aiProcess_GenUVCoords |  // UV 생성
		aiProcess_CalcTangentSpace |  // 탄젠트 생성
		aiProcess_LimitBoneWeights |  // 본의 영향을 받는 정점의 최대 개수를 4개로 제한
		aiProcess_ConvertToLeftHanded;   // 왼손 좌표계로 변환

	const aiScene* m_fbxModel = importer.ReadFile(filePath, importFlags);

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
		m_Materials[i].Create(m_fbxModel->mMaterials[i]);
	}

	// 각 애니메이션 벡터에 애니메이션 클립 생성하고 그 안에서 정보들 바인딩
	// ToDo 어떤 애니메이션을 플레이 하는지 비교해야함.
	for (int i = 0; i < m_fbxModel->mNumAnimations; i++)
	{
		if (m_fbxModel->mAnimations[i] != nullptr)
			m_Animation.Create(m_fbxModel->mAnimations[i]);
	}

	// vertex , index 정보 바인딩
	for (int i = 0; i < m_fbxModel->mNumMeshes; i++)
	{
		if (modelType == ModelType::STATIC)
			m_Meshes[i].Create(m_fbxModel->mMeshes[i]);
		else if (modelType == ModelType::SKELETAL)
			m_Meshes[i].CreateBoneWeight(m_fbxModel->mMeshes[i]);
	}

	// 노드 순회 하면서 바인딩
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

