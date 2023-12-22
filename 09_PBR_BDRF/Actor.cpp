
#include <list>
#include <d3d11.h>
#include <memory>
#include <string>

#include "Actor.h"

void Actor::Update(float DeltaTime)
{
}

void Actor::Render(ID3D11DeviceContext* pDeviceContext)
{
}

void Actor::SetRootComponent(std::weak_ptr<SceneComponent> pRootComponent)
{
	m_pRootComponent = pRootComponent;
}

std::weak_ptr<SceneComponent> Actor::GetRootComponent() const
{
	return m_pRootComponent;
}
