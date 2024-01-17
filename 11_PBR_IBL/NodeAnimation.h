#pragma once
#include "../Common/Helper.h"
#include <assimp/anim.h>

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

	// ���� ���� �Լ�
	Math::Quaternion SphericalInterpolation(const Math::Quaternion& start, const Math::Quaternion& end, float factor);

	void Create(aiNodeAnim* nodeAnimation, double tickPerSecond);
};

class Animation
{
public:
	Animation() {};
	~Animation() {};

	void Create(const std::string strFBXFilePath, const aiAnimation* pAiAnimation);
	void Create(const std::string strFBXFilePath);

public:
	std::vector<NodeAnimation> m_NodeAnims;
	std::string FilePath;
	std::string Name;

	double m_Duration = 0;
	double m_TicksPerSecond = 0;
};

// Animation
// �ϳ��� �ִϸ��̼� Ŭ�� 
// 
// NodeAnimation
// �ִϸ��̼� Ŭ�� ������ ���� Ư�� ���� ���� �ִϸ��̼�
// 
// AnimationKey
// �ִϸ��̼� Ŭ�� ������ ���� ��ȯ ������ �����ϰ� ����
//
// mChannels 
// ���� ���� ���� �ִϸ��̼� ���� ����
//
//