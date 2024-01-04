#pragma once
#include <map>
#include <memory>
#include <string>

class MaterialTexture;
class SkeletalMeshSceneResource;
class Model;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	
	static ResourceManager* Instance;

	std::map<std::wstring, std::weak_ptr<MaterialTexture>> m_MaterialTextureMap;
	std::map<std::string, std::weak_ptr<Model>> m_StaticModelMap;
 
	std::shared_ptr<MaterialTexture> CreateMaterialTexture(std::wstring filePath);
	std::shared_ptr<Model> CreateModelResource(std::string filePath);
};
