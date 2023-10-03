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

    // + ���� ���迡�� ���� ��û �ڵ� �ʿ�
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
    // �浹ü�� �浹�� �����ؾ��ϴ� ��ü���� Ȯ��

    // �浹 ���̾��� ��ü�� �ִ��� Ȯ��
    auto iter = find_if(m_listColliderObject.begin(), m_listColliderObject.end(), 
        [&pDst](pair_collider pSrc) {
            return pDst == pSrc.first;
        });

    // �浹 ���� �Լ� �۵�
    if (iter != m_listColliderObject.end())
    {
        CollisionEntered(pDst);
        m_listColliderObject.push_back(pair_collider(pDst, true));
    }
    else
        iter->second = true;
    
    // CollideEvent �ߵ�
    if (m_fnCollide)
    {
        // ���� ��ü�� �־�� �ش� ��ü�� ���ο��� �Ѱ��ش�.
        if (CGameObject* pObj = pDst->Get_Owner())
        {
            m_fnCollide(pObj);
        }
    }
        
}

void CColliderComponent::CollisionEntered(CColliderComponent* pDst)
{
    // CollisionEntered �ߵ�
    if (m_fnCollisionEntered)
    {
        // ���� ��ü�� �־�� �ش� ��ü�� ���ο��� �Ѱ��ش�.
        if (CGameObject* pObj = pDst->Get_Owner())
        {
            m_fnCollisionEntered(pObj);
        }
    }
}

void CColliderComponent::CollisionExited()
{
    // ������ �ִ� �浹 ������ Ȯ���ϰ�
    for (auto iter = m_listColliderObject.begin(); iter != m_listColliderObject.end(); ++iter)
    {
        if (!iter->second)
        {
            // �� �浹���Ž� �̺�Ʈ�� ���� CollisionExited �ߵ�
            if (m_fnCollisionEntered)
            {
                // ���� ��ü�� �־�� �ش� ��ü�� ���ο��� �Ѱ��ش�.
                if (CGameObject* pObj = (*iter).first->Get_Owner())
                {
                    m_fnCollisionEntered(pObj);
                }
            }
        }
    }
}

