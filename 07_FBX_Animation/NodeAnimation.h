#pragma once
#include "../Common/Helper.h"

namespace Math = DirectX::SimpleMath;

struct AnimationKey
{
	double m_Time;
	Math::Vector3 m_Position;
	Math::Quaternion m_Rotation;
	Math::Vector3 m_Scaling;
};

class NodeAnimation
{
public:
	std::string m_NodeName;
	std::vector<AnimationKey> m_AnimationKey;

public:
	void Evaluate(
		float progressTime,
		Math::Vector3& position,
		Math::Quaternion& rotation,
		Math::Vector3& scaling
	);	
};

