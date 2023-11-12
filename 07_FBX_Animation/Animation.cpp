#include "Animation.h"
#include <assimp\anim.h>
#include "Node.h"
#include <algorithm>
namespace Math = DirectX::SimpleMath;

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

			animKey.m_Time = aiAnim->mChannels[i]->mPositionKeys[j].mTime;
			animKey.m_Rotation = rotation;
			animKey.m_Position = position;
			animKey.m_Scaling = scaling;

			m_NodeAnims[i].m_AnimationKey.push_back(animKey);			
		}
	}
}
