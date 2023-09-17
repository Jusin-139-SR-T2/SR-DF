#include "ColliderComponent.h"

#include "PHY_Shape.h"

CColliderComponent::CColliderComponent()
{
}

CColliderComponent::CColliderComponent(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CColliderComponent::CColliderComponent(const CColliderComponent& rhs)
    : Base(rhs)
{
    m_pShape = rhs.m_pShape->Clone();

}

CColliderComponent::~CColliderComponent()
{
}

void CColliderComponent::Free()
{
    SUPER::Free();

    Safe_Release(m_pShape);
}
