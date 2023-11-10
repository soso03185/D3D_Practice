#include <Windows.h>

#include "NodeAnimation.h"

// Animaiton 보간 작업
void NodeAnimation::Evaluate(float progressTime, Math::Vector3& position, Math::Quaternion& rotation, Math::Vector3& scaling)
{
	static int frameIdx = 5;
	static float durationTime = 0;

	 if (GetAsyncKeyState('E') & 0x8000)
	 {
	 	frameIdx++;
	 	Sleep(100);
	 }

	//	durationTime += progressTime;
	// if(durationTime > 0.1f)
	// { 
	// 	 frameIdx++;
	// 	 durationTime = 0;
	// }	

	 if (frameIdx >= m_AnimationKey.size())
	 {
		 frameIdx = 0;
	 }

	position = m_AnimationKey[frameIdx].m_Position;
	rotation = m_AnimationKey[frameIdx].m_Rotation;
	scaling = m_AnimationKey[frameIdx].m_Scaling;
}