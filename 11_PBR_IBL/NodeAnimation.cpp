// #include <Windows.h>
#include "NodeAnimation.h"
#include <assimp/quaternion.h>
#include <filesystem>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

// Animaiton ���� �۾�
Math::Matrix NodeAnimation::Evaluate(float progressTime)
{
	assert(m_AnimationKey.size() > 0);

	Math::Vector3 position;
	Math::Quaternion rotation;
	Math::Vector3 scaling;

	if (m_AnimationKey.size() == 1)
	{
		position = m_AnimationKey[0].m_Position;
		rotation = m_AnimationKey[0].m_Rotation;
		scaling = m_AnimationKey[0].m_Scaling;
	}
	else
	{	
		size_t i = 0;

		for (i = 0; i < m_AnimationKey.size(); i++)
		{
			UINT lastIndex = m_AnimationKey.size() - 1;

			// for animation change
			if (progressTime > m_AnimationKey[lastIndex].m_Time)
			{
				progressTime = (float)fmod(progressTime, m_AnimationKey[lastIndex].m_Time);
			}

			if (m_AnimationKey[i].m_Time > progressTime)
			{
				break;
			}
		}

		if (i == 0)
		{
			position = m_AnimationKey[i].m_Position;
			rotation = m_AnimationKey[i].m_Rotation;
			scaling = m_AnimationKey[i].m_Scaling;
		}
		else if (i == m_AnimationKey.size())
		{
			position = m_AnimationKey[i - 1].m_Position;
			rotation = m_AnimationKey[i - 1].m_Rotation;
			scaling = m_AnimationKey[i - 1].m_Scaling;
		}
		else
		{
			float t = (progressTime - m_AnimationKey[i - 1].m_Time) / (m_AnimationKey[i].m_Time - m_AnimationKey[i - 1].m_Time);
			position = Math::Vector3::Lerp(m_AnimationKey[i - 1].m_Position, m_AnimationKey[i].m_Position, t);
			rotation = Math::Quaternion::Slerp(m_AnimationKey[i - 1].m_Rotation, m_AnimationKey[i].m_Rotation, t);
			scaling = Math::Vector3::Lerp(m_AnimationKey[i - 1].m_Scaling, m_AnimationKey[i].m_Scaling, t);
		}
	}

	Math::Matrix transform = 
	Math::Matrix::CreateScale(scaling) *
	Math::Matrix::CreateFromQuaternion(rotation) *
	Math::Matrix::CreateTranslation(position);
	return transform;
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

	/// �� ������ �ϳ��� ������ �ִϸ��̼� ///	

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

	//			aiNodeAnim �� ���� �߿�..
	//			aiAnimBehaviour mPreState;     // Ű������ ���� ���� ����
	//			aiAnimBehaviour mPostState;    // Ű������ ���� ���� ����
}

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
