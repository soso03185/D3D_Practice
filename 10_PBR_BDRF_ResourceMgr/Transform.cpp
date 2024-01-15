#include "Transform.h"

Transform::Transform()
{
	m_Local = Math::Matrix::Identity;
	m_World = Math::Matrix::Identity;
	m_pParent = nullptr;
}

Transform::~Transform()
{
}

void Transform::Update(float DeltaTime)
{
	if (m_bDirty)
	{
		m_Local = 
			Math::Matrix::CreateScale(m_LocalScale) * 
			Math::Matrix::CreateFromYawPitchRoll(m_LocalRotation) *
			Math::Matrix::CreateTranslation(m_LocalPosition);
		m_bDirty = false;
	}

	if (m_pParent)
	{
		m_World = m_Local * m_pParent->m_World;
	}
	else
	{
		m_World = m_Local;
	}
	assert(m_Local.Forward().Length() != 0);
}

Math::Vector3 Transform::GetWorldPosition()
{
	return m_World.Translation();
}

//Todo 지금 Local 을 옮겨주고 있는데 나중에 매핑해서 World pos를 옮기는걸로 바꾸기
void Transform::SetLocalPosition(const Math::Vector3& val)
{
	m_bDirty = true;
	m_LocalPosition = val;
}

void Transform::SetLocalRotation(const Math::Vector3& val)
{
	m_bDirty = true;
	m_LocalRotation = val;
}

void Transform::SetLocalScale(const Math::Vector3& val)
{
	m_bDirty = true;
	m_LocalScale = val;
}

void Transform::SetLocalTransform(Math::Matrix val)
{
	m_bDirty = true;
	Math::Quaternion q;
	val.Decompose(m_LocalScale, q, m_LocalPosition);
	m_LocalRotation = q.ToEuler();
}
