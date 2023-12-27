#include "../Common/Helper.h"
#include "ResourceManager.h"
#include "Material.h"
#include "mesh.h"
#include <filesystem>

ResourceManager* ResourceManager::Instance = nullptr;


std::shared_ptr<MaterialTexture> ResourceManager::CreateMaterialTexture(std::wstring filePath)
{
	auto it = m_MaterialTextureMap.find(filePath);
	if (it != m_MaterialTextureMap.end())
	{
		std::shared_ptr<MaterialTexture> resourcePtr = it->second.lock();
		if (resourcePtr)  //UseCount�� 1�̻��̶� �޸𸮰� ���� ����ִٸ� resourcePtr�� �����Ѵ�.
		{
			return resourcePtr;
		}
		else  //UseCount�� 0�̶�� �޸𸮰� �̹� �����Ǿ����Ƿ� �ʿ��� �����Ѵ�.
		{
			m_MaterialTextureMap.erase(it);
			// �������� �ʰ� �Ʒ����� ���� ���� �����Ѵ�.
		}
	}

	std::shared_ptr<MaterialTexture> pTextureRV = std::make_shared<MaterialTexture>();
	pTextureRV->Create(filePath);
	m_MaterialTextureMap[filePath] = pTextureRV;
	return pTextureRV;
}

ResourceManager::ResourceManager()
{
	assert(Instance == nullptr);
	Instance = this;
}

ResourceManager::~ResourceManager()
{

}

std::shared_ptr<SkeletalMeshSceneResource> ResourceManager::CreateSkeletalMeshSceneResource(std::string filePath)
{
	// Ű�� �̹� ������� ���͸����� �ִ��� ã�´�.
	std::string key = filePath;
	auto it = m_SkeletalMeshSceneMap.find(key);
	if (it != m_SkeletalMeshSceneMap.end())
	{
		std::shared_ptr<SkeletalMeshSceneResource> resourcePtr = it->second.lock();
		if (resourcePtr)  //UseCount�� 1�̻��̶� �޸𸮰� ���� ����ִٸ� resourcePtr�� �����Ѵ�.
		{
			return resourcePtr;
		}
		else  //UseCount�� 0�̶�� �޸𸮰� �̹� �����Ǿ����Ƿ� �ʿ��� �����Ѵ�.
		{
			m_SkeletalMeshSceneMap.erase(it);
			// �������� �ʰ� �Ʒ����� ���� ���� �����Ѵ�.
		}
	}

	std::filesystem::path path = ToWString(string(filePath));
	if (!std::filesystem::exists(path)) {
		LOG_MESSAGEA("Erorr file not found: %s", filePath.c_str());
		return nullptr;
	};

	GameTimer timer;
	timer.Tick();
	std::shared_ptr<SkeletalMeshSceneResource> pSceneResource = std::make_shared<SkeletalMeshSceneResource>();
	pSceneResource->Create(filePath.c_str());
	m_SkeletalMeshSceneMap[key] = pSceneResource;
	timer.Tick();
	LOG_MESSAGEA("Complete file: %s %f", filePath.c_str(), timer.DeltaTime());
	return pSceneResource;
}