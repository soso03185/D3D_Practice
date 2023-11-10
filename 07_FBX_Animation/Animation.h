#pragma once
#include "NodeAnimation.h"

class Node;
struct aiAnimation;

class Animation
{
public:
	Animation() {};
	~Animation() {};

	void Create(aiAnimation* aiAnim);

public:
	std::vector<NodeAnimation> m_NodeAnims;
	double m_Duration;
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