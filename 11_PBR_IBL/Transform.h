#pragma once
#include <string>
#include <directxtk/SimpleMath.h>

namespace Math = DirectX::SimpleMath;

class Transform
{
public:
	Transform();
	~Transform();

	Transform* m_pParent = nullptr;
	std::string m_Name;
	Math::Matrix m_Local;
	Math::Matrix m_World;
// ���ε����� ����� �߰�

	void SetParent(Transform* pParent) {
		m_pParent = pParent;
	};
	virtual void Update(float DeltaTime);

	Math::Vector3 GetWorldPosition();


	void SetLocalPosition(const Math::Vector3& val);
	void SetLocalRotation(const Math::Vector3& val);
	void SetLocalScale(const Math::Vector3& val);

	void SetLocalTransform(Math::Matrix val);
	bool m_bDirty = false;

private:
	Math::Vector3 m_LocalPosition = Math::Vector3(0.0f, 0.0f, 0.0f);
	Math::Vector3 m_LocalScale = Math::Vector3(1.0f, 1.0f, 1.0f);
	Math::Vector3 m_LocalRotation = Math::Vector3(0.0f, 0.0f, 0.0f);	// radian
};

