// #include <Windows.h>
#include "NodeAnimation.h"
#include <assimp/quaternion.h>

// Animaiton ���� �۾�
void NodeAnimation::Evaluate(float progressTime, Math::Vector3& position, Math::Quaternion& rotation, Math::Vector3& scaling)
{
	durationTime += progressTime;

	if (frameIndex + 1 >= m_AnimationKey.size())
	{
		frameIndex = 0;
		durationTime = 0;
	}
	
	// ����� ���� Ű������
	const AnimationKey& currentKey = m_AnimationKey[frameIndex];
	const AnimationKey& nextKey = m_AnimationKey[frameIndex + 1];

	if (durationTime > nextKey.m_Time)
	{
		frameIndex++;
	}

	float factor = (durationTime - currentKey.m_Time) / (nextKey.m_Time - currentKey.m_Time);	

	// ���� ���� ����
	position = Math::Vector3::Lerp(currentKey.m_Position, nextKey.m_Position, factor);
	rotation = SphericalInterpolation(currentKey.m_Rotation, nextKey.m_Rotation, factor);
	scaling = Math::Vector3::Lerp(currentKey.m_Scaling, nextKey.m_Scaling, factor);
}

// ���� ���� �Լ�
Math::Quaternion NodeAnimation::SphericalInterpolation(const Math::Quaternion& start, const Math::Quaternion& end, float factor) 
{
	Math::Quaternion result;

	// DirectX::SimpleMath::Quaternion������ Squad �Լ��� ���� SLERP�� �����մϴ�.
	Math::Quaternion::Slerp(start, end, factor, result);

	// Normalize�� �ʿ��� �� �ֽ��ϴ�.
	result.Normalize();

	return result;
}

void AnimationResource::Create(const std::string strFBXFilePath, const aiAnimation* pAiAnimation)
{
}

void AnimationResource::Create(const std::string strFBXFilePath)
{
}
