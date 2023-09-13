#include "ActorComponent.h"

CActorComponent::CActorComponent()
{
}

CActorComponent::CActorComponent(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CActorComponent::CActorComponent(const CActorComponent& rhs)
    : Base(rhs)
{
}

CActorComponent::~CActorComponent()
{
}

void CActorComponent::Free()
{
    SUPER::Free();
}
