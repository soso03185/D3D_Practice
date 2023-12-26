#pragma once
#include <map>
#include <memory>
#include <string>

class MaterialTexture;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	
	static ResourceManager* Instance;

	 std::map<std::wstring, std::weak_ptr<MaterialTexture>> m_MaterialTextureMap;

	std::shared_ptr<MaterialTexture> CreateMaterialTexture(std::wstring filePath);
};
