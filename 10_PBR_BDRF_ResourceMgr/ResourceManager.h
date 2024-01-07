#pragma once
#include <map>
#include <memory>
#include <string>
#include "ModelResource.h"

class MaterialTexture;
class SkeletalMeshSceneResource;
//class ModelResource;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	
	static ResourceManager* Instance;

	std::map<std::wstring, std::weak_ptr<MaterialTexture>> m_MaterialTextureMap;
	std::map<std::string, std::weak_ptr<ModelResource>> m_StaticModelMap;
	std::map<std::string, std::weak_ptr<ModelResource>> m_SkeletalModelMap;

	std::shared_ptr<MaterialTexture> CreateMaterialTexture(std::wstring filePath);
	std::shared_ptr<ModelResource> CreateModelResource(std::string filePath, ModelType modelType);
};
