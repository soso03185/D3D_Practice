#pragma once
#include <map>
#include <memory>
#include <string>

class ResourceManager
{
	ResourceManager();
	~ResourceManager();

	static ResourceManager* Instance;

	//std::map<std::string, std::weak_ptr<SkeletalMeshSceneResource>> m_SkeletalMeshSceneMap;

};
