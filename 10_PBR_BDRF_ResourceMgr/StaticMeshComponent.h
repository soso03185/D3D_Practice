#pragma once
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <wrl.h>

#include "StaticMeshInstance.h"

class Material;
class Model;

class StaticMeshComponent
{
public:
	StaticMeshComponent();
	~StaticMeshComponent();

	std::string m_SceneFilePath; // BeginPlay에서 로딩
	std::shared_ptr<Model>  m_SceneResource;
	std::vector<StaticMeshInstance> m_MeshInstances;

	std::list< StaticMeshComponent*>::iterator m_iterator;

	Math::Matrix m_World;

//	virtual void Update(float DeltaTime) override;

	bool ReadSceneResourceFromFBX(std::string filePath);
	void SetSceneResource(std::shared_ptr<Model> val);

	Material* GetMaterial(UINT index);

	virtual void OnBeginPlay();
	virtual void OnEndPlay();
};

