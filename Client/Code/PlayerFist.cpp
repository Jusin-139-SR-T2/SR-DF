#include "stdafx.h"
#include "PlayerFist.h"
#include "Player.h"

CPlayerFist::CPlayerFist(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayerAttackUnion(pGraphicDev)
{
}

CPlayerFist::CPlayerFist(const CPlayerFist& rhs)
	: CPlayerAttackUnion(rhs)
{
}

CPlayerFist::~CPlayerFist()
{
}

HRESULT CPlayerFist::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// �浹
	m_pTransformComp->Readjust_Transform();
	pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = 1.f;

	// �÷��̾ �ٶ󺸴� ������ ���ư�����
	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	// �߻� ���� ����
	m_tFist.vDir = m_pPlayerTransformcomp->Get_Look();

	// �÷��̾� �޾ƿ���
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	m_bDbugFrame = pPlayer->Get_DBugFrame();

	// ������ ����
	m_tFist.fDamage = 50.f;

	return S_OK;
}

_int CPlayerFist::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	//m_pTransformComp->Move_Pos(&m_tFist.vDir, fTimeDelta, m_tFist.fMoveSpeed);

	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 

	m_tFist.fDeleteTime += 1.f * fTimeDelta;

	if (m_tFist.fDeleteTime >= 2.5f)
	{
		// �Ѿ� ����
		Set_Dead(); //����ü�� ����� 
		m_tFist.fDeleteTime = 0.f;
	}

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CPlayerFist::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CPlayerFist::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	if (*m_bDbugFrame)
	{
		MeshSphereColider(pShape->fRadius, 30, 30);
	}

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CPlayerFist* CPlayerFist::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vDir, CAceUnit* _Owner, PLAYER_ATTACK_STATE _AttackState, ETEAM_ID _eTeamID)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("PlayerBullet Create Failed");
		return nullptr;
	}

	_float fMovePos = 1.5f;
	_vec3 vAttackPosition = vPos + vDir * fMovePos;

	// �÷��̾� ��ġ���� ����
	//pInstance->m_pTransformComp->Set_Pos(vPos.x * vDir.x, vPos.y * vDir.y, vPos.z * vDir.z);	// ���� ��ġ
	pInstance->m_pTransformComp->Set_Pos(vAttackPosition.x, vAttackPosition.y, vAttackPosition.z);	// ���� ��ġ
	pInstance->Set_Owner(_Owner);									// ������ ����
	pInstance->Set_Player_AttackState(_AttackState);			 	// ������ ����(���� ����)
	pInstance->Set_TeamID(_eTeamID);								// ������ �� ����
	pInstance->Set_AttackDir(vDir);

	return pInstance;
}

HRESULT CPlayerFist::Add_Component()
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
	m_pColliderComp->Set_CollisionMask(LAYER_MONSTER | LAYER_BOSSMONSTER); // ��� �浹�ؾ��ϴ� ���̾�� (����)
	m_pColliderComp->Get_CollisionMask();
	return S_OK;
}

void CPlayerFist::Free()
{
	SUPER::Free();
}

void CPlayerFist::OnCollision(CGameObject* pDst, const FContact* const pContact)
{


}

void CPlayerFist::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
	OutputDebugString(L"�÷��̾��� �ָ� �浹 \n");

	// ���� ����  (������, �� ������ ���� Ÿ��, �� ������ ����)
	Change_MonsterHp(-m_tFist.fDamage, pDst, m_ePlayer_AttackState);
	
	// Test �ɱ� ���� Ȯ��
	if (m_ePlayer_AttackState == PSITDONW_ATTACK)
	{
		OutputDebugString(L"�÷��̾ �ɾƼ� ������ \n");
	}

	// �Ѿ� ����
	Set_Dead(); //����ü�� ����� 
}

void CPlayerFist::OnCollisionExited(CGameObject* pDst)
{
}
