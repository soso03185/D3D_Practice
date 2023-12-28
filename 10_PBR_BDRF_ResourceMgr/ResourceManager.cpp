
#include "../Common/Helper.h"
#include "ResourceManager.h"
#include "Material.h"
#include "mesh.h"
#include "Model.h"

#include <filesystem>
#include "..\Common\GameTimer.h"

ResourceManager* ResourceManager::Instance = nullptr;


std::shared_ptr<MaterialTexture> ResourceManager::CreateMaterialTexture(std::wstring filePath)
{
	auto it = m_MaterialTextureMap.find(filePath);
	if (it != m_MaterialTextureMap.end())
	{
		std::shared_ptr<MaterialTexture> resourcePtr = it->second.lock();
		if (resourcePtr)  //UseCount가 1이상이라 메모리가 아직 살아있다면 resourcePtr를 리턴한다.
		{
			return resourcePtr;
		}
		else  //UseCount가 0이라면 메모리가 이미 해제되었으므로 맵에서 제거한다.
		{
			m_MaterialTextureMap.erase(it);
			// 리턴하지 않고 아래에서 새로 만들어서 리턴한다.
		}
	}

	std::filesystem::path path = filePath;
	if (!std::filesystem::exists(path)) {
		LOG_MESSAGEA("Error file not found: %s", path.string().c_str());
		return nullptr;
	};

	std::shared_ptr<MaterialTexture> pTextureRV = std::make_shared<MaterialTexture>();
	pTextureRV->Create(filePath);
	m_MaterialTextureMap[filePath] = pTextureRV;
	return pTextureRV;
}

std::shared_ptr<Model> ResourceManager::CreateModelResource(std::string filePath)
{
	// 키로 이미 만들어진 머터리얼이 있는지 찾는다.
	std::string key = filePath;
	auto it = m_StaticModelMap.find(key);
	if (it != m_StaticModelMap.end())
	{
		std::shared_ptr<Model> resourcePtr = it->second.lock();
		if (resourcePtr)  //UseCount가 1이상이라 메모리가 아직 살아있다면 resourcePtr를 리턴한다.
		{
			return resourcePtr;
		}
		else  //UseCount가 0이라면 메모리가 이미 해제되었으므로 맵에서 제거한다.
		{
			m_StaticModelMap.erase(it);
			// 리턴하지 않고 아래에서 새로 만들어서 리턴한다.
		}
	}

	std::filesystem::path path = ToWString(string(filePath));
	if (!std::filesystem::exists(path)) {
		LOG_MESSAGEA("Erorr file not found: %s", filePath.c_str());
		return nullptr;
	};

	GameTimer timer;
	timer.Tick();
	std::shared_ptr<Model> pSceneResource = std::make_shared<Model>();
	pSceneResource->ReadFile(filePath.c_str());
	m_StaticModelMap[key] = pSceneResource;
	timer.Tick();
	LOG_MESSAGEA("Complete file: %s %f", filePath.c_str(), timer.DeltaTime());
	return pSceneResource;
}

ResourceManager::ResourceManager()
{
	assert(Instance == nullptr);
	Instance = this;
}

ResourceManager::~ResourceManager()
{

}