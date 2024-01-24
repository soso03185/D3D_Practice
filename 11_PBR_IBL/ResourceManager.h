#pragma once
#include <map>
#include <memory>
#include <string>
#include "ModelResource.h"

class MaterialTexture;
class Animation;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	
	static ResourceManager* Instance;

	std::map<std::wstring, std::weak_ptr<MaterialTexture>> m_MaterialTextureMap;
	std::map<std::string, std::weak_ptr<ModelResource>> m_StaticModelMap;
	std::map<std::string, std::weak_ptr<ModelResource>> m_SkeletalModelMap;
	std::map<std::string, std::weak_ptr<Animation>> m_AnimationMap;	// Animation


	std::shared_ptr<MaterialTexture> CreateMaterialTexture(std::wstring filePath);
	std::shared_ptr<ModelResource> CreateStaticModelResource(std::string filePath);
	std::shared_ptr<ModelResource> CreateSkeletalModelResource(std::string filePath);
	std::shared_ptr<Animation> CreateAnimationResource(std::string filePath);

	// ���� ���͸��� �������� ���� �ؽ�ó�� ����Ҽ� �����Ƿ� ���� ��Ͽ��� ���ϰ�� Ȯ���ؼ� ������
	std::shared_ptr<MaterialTexture> CreateTextureResource(std::wstring filePath);
};
