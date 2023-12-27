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

	int frameIndex = 0;
	float durationTime = 0;

public:
	void Evaluate(
		float progressTime,
		Math::Vector3& position,
		Math::Quaternion& rotation,
		Math::Vector3& scaling
	);	

	// 구면 보간 함수
	Math::Quaternion SphericalInterpolation(const Math::Quaternion& start, const Math::Quaternion& end, float factor);
};
