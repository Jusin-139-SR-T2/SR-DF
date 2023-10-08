
#include "stdafx.h"
#include "MonsterPunch.h"

CMonsterPunch::CMonsterPunch(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CMonsterPunch::CMonsterPunch(const CMonsterPunch& rhs)
	: CGameObject(rhs)
{
}

CMonsterPunch::~CMonsterPunch()
{
}

HRESULT CMonsterPunch::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// �浹
	m_pTransformComp->Readjust_Transform();
	FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = 1.5f;

	//����׿� �ؽ���
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Debug", L"Sphere");

	return S_OK;
}

_int CMonsterPunch::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CMonsterPunch::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CMonsterPunch::Render_GameObject()
{
}

CMonsterPunch* CMonsterPunch::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("ThrowPipe Create Failed");
		return nullptr;
	}

	// �����Ҷ� ���� ��ġ �� �����ϱ� ���� Create���� �ʱ���ġ�� ����� 
	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);

	return pInstance;
}

HRESULT CMonsterPunch::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_ProjectileTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);


	// -------------------- �浹 ��Ʈ --------------------------
	// �ݶ��̴� ������Ʈ
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);
	// ���� ���� ���
	m_pColliderComp->EnterToPhysics(0);

	// �浹 �Լ� ����
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// �浹 ���̾�, ����ũ ����
	m_pColliderComp->Set_CollisionLayer(ELAYER_PROJECTILE); // �� Ŭ������ ���� �浹���̾� 
	m_pColliderComp->Set_CollisionMask(ELAYER_PLAYER); // ��� �浹�ؾ��ϴ� ���̾�� 

	return S_OK;
}

void CMonsterPunch::Free()
{
	SUPER::Free();
}

void CMonsterPunch::Billboard(const _float& fTimeDelta)
{

}

void CMonsterPunch::OnCollision(CGameObject* pDst)
{


}

void CMonsterPunch::OnCollisionEntered(CGameObject* pDst)
{
	OutputDebugString(L"������ Debug Attack�� �浹���� \n");

	Set_Dead(); //����ü�� ����� 
}

void CMonsterPunch::OnCollisionExited(CGameObject* pDst)
{
}
