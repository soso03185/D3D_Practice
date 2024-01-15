// #include <Windows.h>
#include "NodeAnimation.h"
#include <assimp/quaternion.h>
#include <filesystem>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

// Animaiton ���� �۾�
void NodeAnimation::Evaluate(float progressTime, Math::Vector3& position, Math::Quaternion& rotation, Math::Vector3& scaling)
{
	assert(m_AnimationKey.size() > 0);
	durationTime += progressTime;

	if (m_AnimationKey.size() == 1)
	{
		position = m_AnimationKey[0].m_Position;
		rotation = m_AnimationKey[0].m_Rotation;
		scaling = m_AnimationKey[0].m_Scaling;
	}
	else
	{
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

void NodeAnimation::Create(aiNodeAnim* nodeAnimation, double tickPerSecond)
{
	assert(nodeAnimation != nullptr);
	assert(nodeAnimation->mNumPositionKeys == nodeAnimation->mNumRotationKeys);
	assert(nodeAnimation->mNumRotationKeys == nodeAnimation->mNumScalingKeys);

	m_NodeName = nodeAnimation->mNodeName.C_Str();
	//LOG_MESSAGEA(NodeName.c_str());
	size_t numkeys = nodeAnimation->mNumPositionKeys;
	m_AnimationKey.resize(numkeys);

	for (size_t i = 0; i < numkeys; i++)
	{
		aiVectorKey& pos = nodeAnimation->mPositionKeys[i];
		aiQuatKey& rot = nodeAnimation->mRotationKeys[i];
		aiVectorKey& scl = nodeAnimation->mScalingKeys[i];

		assert(pos.mTime == rot.mTime);
		assert(rot.mTime == scl.mTime);

		m_AnimationKey[i].m_Time = (float)(pos.mTime / tickPerSecond);
		m_AnimationKey[i].m_Position = { pos.mValue.x,pos.mValue.y,pos.mValue.z };
		m_AnimationKey[i].m_Rotation = { rot.mValue.x,rot.mValue.y,rot.mValue.z, rot.mValue.w };
		m_AnimationKey[i].m_Scaling = { scl.mValue.x,scl.mValue.y,scl.mValue.z };
	}
}

//void Animation::Create(aiAnimation* aiAnim)
//{
//	/// �� ������ �ϳ��� ������ �ִϸ��̼� ///	
//
//	m_Duration = aiAnim->mDuration;
//	m_TicksPerSecond = aiAnim->mTicksPerSecond;
//
//	m_NodeAnims.resize(aiAnim->mNumChannels);
//
//	for (int i = 0; i < aiAnim->mNumChannels; i++)
//	{
//		m_NodeAnims[i].m_NodeName = aiAnim->mChannels[i]->mNodeName.C_Str();
//
//		// Position , Rotation , Scale
//		for (int j = 0; j < aiAnim->mChannels[i]->mNumPositionKeys; j++)
//		{
//			AnimationKey animKey;
//
//			Math::Vector3 position = Math::Vector3(
//				aiAnim->mChannels[i]->mPositionKeys[j].mValue.x,
//				aiAnim->mChannels[i]->mPositionKeys[j].mValue.y,
//				aiAnim->mChannels[i]->mPositionKeys[j].mValue.z
//			);
//			Math::Quaternion rotation = Math::Quaternion(
//				aiAnim->mChannels[i]->mRotationKeys[j].mValue.x,
//				aiAnim->mChannels[i]->mRotationKeys[j].mValue.y,
//				aiAnim->mChannels[i]->mRotationKeys[j].mValue.z,
//				aiAnim->mChannels[i]->mRotationKeys[j].mValue.w
//			);
//			Math::Vector3 scaling = Math::Vector3(
//				aiAnim->mChannels[i]->mScalingKeys[j].mValue.x,
//				aiAnim->mChannels[i]->mScalingKeys[j].mValue.y,
//				aiAnim->mChannels[i]->mScalingKeys[j].mValue.z
//			);
//
//			//			aiNodeAnim �� ���� �߿�..
//			//			aiAnimBehaviour mPreState;     // Ű������ ���� ���� ����
//			//			aiAnimBehaviour mPostState;    // Ű������ ���� ���� ����
//
//			animKey.m_Time = aiAnim->mChannels[i]->mPositionKeys[j].mTime / aiAnim->mTicksPerSecond;
//			animKey.m_Rotation = rotation;
//			animKey.m_Position = position;
//			animKey.m_Scaling = scaling;
//
//			m_NodeAnims[i].m_AnimationKey.push_back(animKey);
//		}
//	}
//}

// ModelResource�� �Ʒ��� Create()�κ��� ȣ��
void Animation::Create(const std::string strFBXFilePath, const aiAnimation* pAiAnimation)
{
	FilePath = strFBXFilePath;
	m_NodeAnims.resize(pAiAnimation->mNumChannels);

	// ��ü �ð����� = �����Ӽ� / 1�ʴ� �����Ӽ�
	m_Duration = (float)(pAiAnimation->mDuration / pAiAnimation->mTicksPerSecond);
	for (size_t iChannel = 0; iChannel < pAiAnimation->mNumChannels; iChannel++)
	{
		aiNodeAnim* pAiNodeAnim = pAiAnimation->mChannels[iChannel];
		NodeAnimation& refNodeAnim = m_NodeAnims[iChannel];
		
		//ToDo ���� �Լ� �ȸ������.
		refNodeAnim.Create(pAiNodeAnim, pAiAnimation->mTicksPerSecond);
	}
}

// ���ҽ� �Ŵ����κ��� ȣ��
void Animation::Create(const std::string strFBXFilePath)
{
	std::filesystem::path path = ToWString(std::string(strFBXFilePath));

	Assimp::Importer importer;

	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);	// $assimp_fbx$ ��� ��������
	unsigned int importFlags = aiProcess_ConvertToLeftHanded;	// �޼� ��ǥ��� ��ȯ
	const aiScene* scene = importer.ReadFile(strFBXFilePath, importFlags);
	if (!scene) {
		LOG_ERRORA("Error Loading Animation FBX File: %s", importer.GetErrorString());
		return;
	}
	assert(scene->mNumAnimations == 1); // �ִϸ��̼��� ���ų� 1�������Ѵ�. 
	// ����� �ִϸ��̼��� �ϳ��� ��ġ�� ����� FBX export���� NLA��Ʈ��,��� �׼� �ɼ��� ���ϴ�.

	const aiAnimation* pAiAnimation = scene->mAnimations[0];
	// ä�μ��� aiAnimation �ȿ��� �ִϸ��̼� ������  ǥ���ϴ� aiNode�� �����̴�.
	assert(pAiAnimation->mNumChannels > 1); // �ִϸ��̼��� �ִٸ� aiNode �� �ϳ� �̻� �־���Ѵ�.

	Create(strFBXFilePath, pAiAnimation);

	importer.FreeScene();

}
