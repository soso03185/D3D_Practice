#pragma once
#include <wrl.h>
#include <string>
#include <list>
#include <memory>

class ModelResource;
class Node;

class Animator
{ 
public:
	UINT  m_AnimationIndex = 0;
	float m_AnimationProgressTime = 0.0f;
	float m_DurationTime = 0.0f;
	bool m_animChange = false;

	std::shared_ptr<ModelResource>  m_pModelResource;
	Node* m_pRootNode;	
	
	std::list<std::string> m_AnimationFilePath; // BeginPlay에서 로딩	

	void Binding(std::shared_ptr<ModelResource> modelResource, Node* rootNode);
	void Update(float deltaTime);
	void UpdateReference(int index);
	void SetAnimation(int index);
	bool AddSceneAnimationFromFBX(std::string filePath);
 };