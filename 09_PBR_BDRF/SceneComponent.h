#pragma once
#include "Component.h"
#include <directxtk/SimpleMath.h>

namespace Math = DirectX::SimpleMath;

class SceneComponent : public Component
{
public:
	SceneComponent() {};
	SceneComponent(Actor* pOwner, const std::string& Name);
	~SceneComponent();

	std::string m_Name;
	Math::Matrix m_Local;
	Math::Matrix m_World;
	Math::Vector3 m_Position;
	Math::Vector3 m_Rotation;
	Math::Vector3 m_Scale;

	virtual void Update(float DeltaTime);
};

