// #include <Windows.h>
#include "NodeAnimation.h"
#include <assimp/quaternion.h>

// Animaiton 보간 작업
void NodeAnimation::Evaluate(float progressTime, Math::Vector3& position, Math::Quaternion& rotation, Math::Vector3& scaling)
{
	durationTime += progressTime;

	if (frameIndex + 1 >= m_AnimationKey.size())
	{
		frameIndex = 0;
		durationTime = 0;
	}

	// 현재와 다음 키프레임
	const AnimationKey& currentKey = m_AnimationKey[frameIndex];
	const AnimationKey& nextKey = m_AnimationKey[frameIndex + 1];

	if (durationTime > nextKey.m_Time)
	{
		frameIndex++;
	}

	float factor = (durationTime - currentKey.m_Time) / (nextKey.m_Time - currentKey.m_Time);
	
	// 선형 보간 적용
	position = LinearInterpolation(currentKey.m_Position, nextKey.m_Position, factor);
	rotation = SphericalInterpolation(currentKey.m_Rotation, nextKey.m_Rotation, factor);
	scaling = LinearInterpolation(currentKey.m_Scaling, nextKey.m_Scaling, factor);
}

// 선형 보간 함수
Math::Vector3 NodeAnimation::LinearInterpolation(const Math::Vector3& start, const Math::Vector3& end, float factor)
{
	return Math::Vector3::Lerp(start, end, factor);
	// return start + factor * (end - start);
}

// 구면 보간 함수
Math::Quaternion NodeAnimation::SphericalInterpolation(const Math::Quaternion& start, const Math::Quaternion& end, float factor) {

	// 변환용 Quaternion
	Math::Quaternion result;

	// DirectX::SimpleMath::Quaternion에서는 Squad 함수를 통해 SLERP를 수행합니다.
	Math::Quaternion::Slerp(start, end, factor, result);

	// Normalize는 필요할 수 있습니다.
	result.Normalize();

	return result;
}
