#include "ColliderComponent.h"

#include "CollisionPrimitive.h"

#include "Export_System.h"
#include "Export_Utility.h"

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
    // �浹ü ������
    switch (rhs.m_pCollisionShape->Get_Type())
    {
    default:
    case ECOLLISION::SPHERE:
    {
        FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(rhs.m_pCollisionShape);
        m_pCollisionShape = new FCollisionSphere(*pShape);
        break;
    }
    case ECOLLISION::BOX:
    {
        FCollisionBox* pShape = dynamic_cast<FCollisionBox*>(rhs.m_pCollisionShape);
        m_pCollisionShape = new FCollisionBox(*pShape);
        break;
    }
    case ECOLLISION::CAPSULE:
    {
        FCollisionCapsule* pShape = dynamic_cast<FCollisionCapsule*>(rhs.m_pCollisionShape);
        m_pCollisionShape = new FCollisionCapsule(*pShape);
        break;
    }
    }

    m_pCollisionShape->Set_Owner(this);

    // �浹ü�� ���� �̺�Ʈ �߰��ϱ�
    m_pCollisionShape->Set_Event([this](void* pDst) { 
        this->OnCollision(reinterpret_cast<CColliderComponent*>(pDst));
        });

    // �̺�Ʈ �Լ� Ŭ�� ����, �������� �ܺο��� �߰�
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
    ExitFromPhysics(0);
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

    // �浹ü�� ���� ����
    m_pCollisionShape->Set_Owner(this);

    // �浹ü�� ���� �̺�Ʈ �߰��ϱ�
    m_pCollisionShape->Set_Event([this](void* pDst) {
        this->OnCollision(reinterpret_cast<CColliderComponent*>(pDst));
        });

    return S_OK;
}

_int CColliderComponent::Update_Component(const _float& fTimeDelta)
{
    for (auto iter = m_listColliderObject.begin(); iter != m_listColliderObject.end(); ++iter)
        iter->second = false;

    return 0;
}

void CColliderComponent::EnterToPhysics(_uint iIndex)
{
    Engine::Add_ColliderToPhysicsWorld(iIndex, m_pCollisionShape);
}

void CColliderComponent::ExitFromPhysics(_uint iIndex)
{
    Engine::Delete_ColliderToPhysicsWorld(iIndex, m_pCollisionShape);
}

void CColliderComponent::Update_Physics(const _matrix& matWorld)
{
    Readjust_Transform();
    Set_TransformToWorld(matWorld);

    FVector3 vPos;
    vPos.x = Get_PosByTransform().x;
    vPos.y = Get_PosByTransform().y;
    vPos.z = Get_PosByTransform().z;
    m_pCollisionShape->pBody->Set_Position(vPos);
}

void CColliderComponent::OnCollision(CColliderComponent* pDst)
{
    // �浹ü�� �浹�� �����ؾ��ϴ� ��ü���� Ȯ��

    // �浹 ���̾��� ��ü�� �ִ��� Ȯ��
    auto iter = find_if(m_listColliderObject.begin(), m_listColliderObject.end(), 
        [&pDst](pair_collider pSrc) {
            return pDst == pSrc.first;
        });

    // �浹 ���� �Լ� �۵�
    if (iter == m_listColliderObject.end())
    {
        OnCollisionEntered(pDst);
        m_listColliderObject.push_back(pair_collider(pDst, true));
    }
    else
        iter->second = true;
    
    // CollideEvent �ߵ�
    if (m_fnCollision)
    {
        // ���� ��ü�� �־�� �ش� ��ü�� ���ο��� �Ѱ��ش�.
        if (CGameObject* pObj = pDst->Get_Owner())
        {
            m_fnCollision(pObj);
        }
    }
        
}

void CColliderComponent::OnCollisionEntered(CColliderComponent* pDst)
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

void CColliderComponent::OnCollisionExited()
{
    // ������ �ִ� �浹 ������ Ȯ���ϰ�
    for (auto iter = m_listColliderObject.begin(); iter != m_listColliderObject.end(); ++iter)
    {
        if (!iter->second)
        {
            // �� �浹���Ž� �̺�Ʈ�� ���� CollisionExited �ߵ�
            if (m_fnCollisionExited)
            {
                // ���� ��ü�� �־�� �ش� ��ü�� ���ο��� �Ѱ��ش�.
                if (CGameObject* pObj = (*iter).first->Get_Owner())
                {
                    m_fnCollisionExited(pObj);
                }
            }
        }
    }
}

