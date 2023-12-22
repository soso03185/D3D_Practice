#pragma once
class Component;
class SceneComponent;
class World;
class Actor
{
public:
	Actor() {};
	~Actor() {};
public:
	std::list<std::shared_ptr<Component>> m_OwnedComponents;	// ������ ������Ʈ��

public:
	std::weak_ptr<SceneComponent> m_pRootComponent;		// ������Ʈ �߿� ����� ��Ʈ���� ����
	World* m_pOwnerWorld = nullptr;					// �� ���� ������Ʈ�� ���� ����

	virtual void Update(float DeltaTime);
	virtual void Render(ID3D11DeviceContext* pDeviceContext);

	// ��Ʈ ������Ʈ�� ����
	void SetRootComponent(std::weak_ptr<SceneComponent> pRootComponent);
	std::weak_ptr<SceneComponent> GetRootComponent() const;

	// ������Ʈ�� ��Ʋ�� �Լ��� �����մϴ�.
	template<typename T>
	std::weak_ptr<T> CreateComponent(const std::string& Name)
	{
		bool bIsBase = std::is_base_of<Component, T>::value;
		assert(bIsBase == true);
		std::shared_ptr<T> pComponent = std::make_shared<T>();
		m_OwnedComponents.push_back(pComponent);
		return pComponent;
	}

};

