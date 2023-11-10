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
// 하나의 애니메이션 클립 
// 
// NodeAnimation
// 애니메이션 클립 내에서 모델의 특정 뼈에 대한 애니메이션
// 
// AnimationKey
// 애니메이션 클립 내에서 뼈의 변환 정보를 저장하고 관리
//
// mChannels 
// 모델의 뼈에 대한 애니메이션 동작 정보
//
//