#include "ColliderComponent.h"

#include "CollisionPrimitive.h"

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
    //*m_pCollisionShape = *(rhs.m_pCollisionShape);

}

CColliderComponent::~CColliderComponent()
{
}

CComponent* CColliderComponent::Create(LPDIRECT3DDEVICE9 pGraphicDev, ECOLLISION eType)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_Component(pGraphicDev, eType)))
    {
        Safe_Release(pInstance);

        MSG_BOX("Player Create Failed");
        return nullptr;
    }

    return pInstance;
}

CComponent* CColliderComponent::Clone()
{
    return new ThisClass(*this);
}

void CColliderComponent::Free()
{
    SUPER::Free();

    // + 물리 세계에서 제거 요청 코드 필요
    Safe_Delete(m_pCollisionShape);
}

HRESULT CColliderComponent::Ready_Component(LPDIRECT3DDEVICE9 pGraphicDev, ECOLLISION eType)
{
    switch (eType)
    {
    case ECOLLISION::SPHERE:
        m_pCollisionShape = new FCollisionSphere();
        break;
    case ECOLLISION::BOX:
        m_pCollisionShape = new FCollisionBox();
        break;
    case ECOLLISION::CAPSULE:
        m_pCollisionShape = new FCollisionCapsule();
        break;
    }

    return S_OK;
}

_int CColliderComponent::Update_Component(const _float& fTimeDelta)
{
    for (auto iter = m_listColliderObject.begin(); iter != m_listColliderObject.end(); ++iter)
        iter->second = false;

    return 0;
}

void CColliderComponent::Collide(CColliderComponent* pDst)
{
    // 충돌체가 충돌을 진행해야하는 객체인지 확인

    // 충돌 중이었던 객체가 있는지 확인
    auto iter = find_if(m_listColliderObject.begin(), m_listColliderObject.end(), 
        [&pDst](pair_collider pSrc) {
            return pDst == pSrc.first;
        });

    // 충돌 진입 함수 작동
    if (iter != m_listColliderObject.end())
    {
        CollisionEntered(pDst);
        m_listColliderObject.push_back(pair_collider(pDst, true));
    }
    else
        iter->second = true;
    
    // CollideEvent 발동
    if (m_fnCollide)
    {
        // 오너 객체가 있어야 해당 객체를 주인에게 넘겨준다.
        if (CGameObject* pObj = pDst->Get_Owner())
        {
            m_fnCollide(pObj);
        }
    }
        
}

void CColliderComponent::CollisionEntered(CColliderComponent* pDst)
{
    // CollisionEntered 발동
    if (m_fnCollisionEntered)
    {
        // 오너 객체가 있어야 해당 객체를 주인에게 넘겨준다.
        if (CGameObject* pObj = pDst->Get_Owner())
        {
            m_fnCollisionEntered(pObj);
        }
    }
}

void CColliderComponent::CollisionExited()
{
    // 가지고 있던 충돌 정보를 확인하고
    for (auto iter = m_listColliderObject.begin(); iter != m_listColliderObject.end(); ++iter)
    {
        if (!iter->second)
        {
            // 각 충돌제거시 이벤트에 대한 CollisionExited 발동
            if (m_fnCollisionEntered)
            {
                // 오너 객체가 있어야 해당 객체를 주인에게 넘겨준다.
                if (CGameObject* pObj = (*iter).first->Get_Owner())
                {
                    m_fnCollisionEntered(pObj);
                }
            }
        }
    }
}

