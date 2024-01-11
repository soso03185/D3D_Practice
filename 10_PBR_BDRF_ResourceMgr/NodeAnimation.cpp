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

void Animation::Create(aiAnimation* aiAnim)
{
	/// �� ������ �ϳ��� ������ �ִϸ��̼� ///	

	m_Duration = aiAnim->mDuration;
	m_TicksPerSecond = aiAnim->mTicksPerSecond;

	m_NodeAnims.resize(aiAnim->mNumChannels);

	for (int i = 0; i < aiAnim->mNumChannels; i++)
	{
		m_NodeAnims[i].m_NodeName = aiAnim->mChannels[i]->mNodeName.C_Str();

		// Position , Rotation , Scale
		for (int j = 0; j < aiAnim->mChannels[i]->mNumPositionKeys; j++)
		{
			AnimationKey animKey;

			Math::Vector3 position = Math::Vector3(
				aiAnim->mChannels[i]->mPositionKeys[j].mValue.x,
				aiAnim->mChannels[i]->mPositionKeys[j].mValue.y,
				aiAnim->mChannels[i]->mPositionKeys[j].mValue.z
			);
			Math::Quaternion rotation = Math::Quaternion(
				aiAnim->mChannels[i]->mRotationKeys[j].mValue.x,
				aiAnim->mChannels[i]->mRotationKeys[j].mValue.y,
				aiAnim->mChannels[i]->mRotationKeys[j].mValue.z,
				aiAnim->mChannels[i]->mRotationKeys[j].mValue.w
			);
			Math::Vector3 scaling = Math::Vector3(
				aiAnim->mChannels[i]->mScalingKeys[j].mValue.x,
				aiAnim->mChannels[i]->mScalingKeys[j].mValue.y,
				aiAnim->mChannels[i]->mScalingKeys[j].mValue.z
			);

			//			aiNodeAnim �� ���� �߿�..
			//			aiAnimBehaviour mPreState;     // Ű������ ���� ���� ����
			//			aiAnimBehaviour mPostState;    // Ű������ ���� ���� ����

			animKey.m_Time = aiAnim->mChannels[i]->mPositionKeys[j].mTime / aiAnim->mTicksPerSecond;
			animKey.m_Rotation = rotation;
			animKey.m_Position = position;
			animKey.m_Scaling = scaling;

			m_NodeAnims[i].m_AnimationKey.push_back(animKey);
		}
	}
}
