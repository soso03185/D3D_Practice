#pragma once
#include <map>
#include <memory>
#include <string>
#include "ModelResource.h"

class MaterialTexture;
class SkeletalMeshSceneResource;
class AnimationResource;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	
	static ResourceManager* Instance;

	std::map<std::wstring, std::weak_ptr<MaterialTexture>> m_MaterialTextureMap;
	std::map<std::string, std::weak_ptr<ModelResource>> m_StaticModelMap;
	std::map<std::string, std::weak_ptr<ModelResource>> m_SkeletalModelMap;
	std::map<std::string, std::weak_ptr<AnimationResource>> m_AnimationMap;	// Animation


	std::shared_ptr<MaterialTexture> CreateMaterialTexture(std::wstring filePath);
	std::shared_ptr<ModelResource> CreateModelResource(std::string filePath, ModelType modelType);
	std::shared_ptr<AnimationResource> CreateAnimationResource(std::string filePath);

};
