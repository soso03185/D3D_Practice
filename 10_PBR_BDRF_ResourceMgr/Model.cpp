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
	  
	return true;
}

//Material* Model::GetMaterial(UINT index)
//{
//	return &m_Materials[index];
//}