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
		m_Local = Math::Matrix::CreateScale(m_LocalScale) * Math::Matrix::CreateFromYawPitchRoll(m_LocalRotation) * Math::Matrix::CreateTranslation(m_LocalPosition);
		m_bDirty = false;
	}

	if (m_pParent)
	{
		m_World = m_pParent->m_World * m_Local;
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
