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
	m_Anims.clear();
}

void Model::Update(float deltaTime)
{
}

bool Model::ReadFile(ID3D11Device* device, const char* filePath)
{
	Assimp::Importer importer;
	unsigned int importFlags = aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded;

	const aiScene* fbxModel = importer.ReadFile("../Resource/BoxHuman.fbx", importFlags);

	if (!fbxModel)
	{
		LOG_ERRORA("Error loading FBX file: %s", importer.GetErrorString());
		return false;
	}

	m_Materials.resize(fbxModel->mNumMaterials);
	for (int i = 0; i < fbxModel->mNumMaterials; i++)
	{
		// fbx에 적용된 맵(텍스쳐) 들을 가져오는 과정.
		m_Materials[i].Create(device, fbxModel->mMaterials[i]);
	}	

	m_Meshes.resize(fbxModel->mNumMeshes);
	m_RootNode.Create(this, fbxModel->mRootNode);

	for (int i = 0; i < fbxModel->mNumMeshes; i++)
	{
		// vertex , index 정보 바인딩
		m_Meshes[i].Create(device, fbxModel->mMeshes[i]); 
	}

	return true;
}

Material* Model::GetMaterial(UINT index)
{
	return &m_Materials[index];	
}

void Model::Create()
{

}
