#pragma once
#include <string>

class Actor;
class Component
{
public:
	Component() {};
	Component(Actor* pOwner, const std::string& Name) : m_pOwner(pOwner), m_Name(Name)
	{
	}
	virtual ~Component() {};

protected:
	Actor* m_pOwner = nullptr;
	std::string m_Name;			// ���� ������Ʈ �ȿ��� ���� ������ ������Ʈ�� �ٸ� �̸��� �����Ͽ� �����Ҽ� �ְ� �Ѵ�.	
public:
	const std::string& GetName() const { return m_Name; }
	virtual void Update(float DeltaTime) = 0;
	void SetOwner(Actor* pOwner) { m_pOwner = pOwner; }
	Actor* GetOwner() { return m_pOwner; }


};

