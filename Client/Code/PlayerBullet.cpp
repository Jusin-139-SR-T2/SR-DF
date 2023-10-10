#include "stdafx.h"

#include "Player.h"
#include "PlayerBullet.h"

CPlayerBullet::CPlayerBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CPlayerBullet::CPlayerBullet(const CPlayerBullet& rhs)
	: CGameObject(rhs)
{
}

CPlayerBullet::~CPlayerBullet()
{
}

HRESULT CPlayerBullet::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// �浹
	m_pTransformComp->Readjust_Transform();
	FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = 0.5f;

	// �÷��̾ �ٶ󺸴� ������ ���ư�����
	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	m_vDir = m_pPlayerTransformcomp->Get_Look();

	// �÷��̾� �޾ƿ���
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	m_bDbugFrame = pPlayer->Get_DBugFrame();

	return S_OK;
}

_int CPlayerBullet::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	m_pTransformComp->Move_Pos(&m_vDir, fTimeDelta, m_fMoveSpeed);

	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CPlayerBullet::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CPlayerBullet::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	
	if (*m_bDbugFrame)
	{
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

		m_pBufferComp->Render_Buffer();
		
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CPlayerBullet* CPlayerBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float MoveSpeed)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("PlayerBullet Create Failed");
		return nullptr;
	}

	// �÷��̾� ��ġ���� ����
	pInstance->m_pTransformComp->Set_Pos(vPos.x, vPos.y, vPos.z);
	pInstance->m_fMoveSpeed = MoveSpeed; // ����ü �ӵ� ����

	return pInstance;
}

HRESULT CPlayerBullet::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
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
	m_pColliderComp->Set_CollisionLayer(LAYER_PLAYER_ATTACK); // �� Ŭ������ ���� �浹���̾� (�÷��̾� ����)
	m_pColliderComp->Set_CollisionMask(LAYER_MONSTER | LAYER_WALL); // ��� �浹�ؾ��ϴ� ���̾�� (����)

	return S_OK;
}

void CPlayerBullet::Free()
{
	SUPER::Free();
}

void CPlayerBullet::OnCollision(CGameObject* pDst)
{


}

void CPlayerBullet::OnCollisionEntered(CGameObject* pDst)
{
	OutputDebugString(L"�÷��̾��� �Ѿ� �浹 \n");

	Set_Dead(); //����ü�� ����� 
}

void CPlayerBullet::OnCollisionExited(CGameObject* pDst)
{
}
