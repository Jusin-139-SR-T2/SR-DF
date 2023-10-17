#include "stdafx.h"

#include "Player.h"
#include "PlayerBullet.h"

CPlayerBullet::CPlayerBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayerAttackUnion(pGraphicDev)
{
}

CPlayerBullet::CPlayerBullet(const CPlayerBullet& rhs)
	: CPlayerAttackUnion(rhs)
{
}

CPlayerBullet::~CPlayerBullet()
{

}

HRESULT CPlayerBullet::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	Set_TeamID(ETEAM_PLAYER);

	// 충돌
	m_pTransformComp->Readjust_Transform();
	pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = 0.5f;

	// 플레이어가 바라보는 쪽으로 날아가게함
	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	// 발사 방향 설정
	//m_vDir = m_pPlayerTransformcomp->Get_Look();

	// 플레이어 받아오기
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	m_bDbugFrame = pPlayer->Get_DBugFrame();

	// 데미지 설정
	m_tBullet.fDamage = 55.f;

	return S_OK;
}

_int CPlayerBullet::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);
	
	m_pTransformComp->Move_Pos(&m_vDir, fTimeDelta, m_tBullet.fMoveSpeed);

	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	m_tBullet.fDeleteTime += 1.f * fTimeDelta;

	if (m_tBullet.fDeleteTime >= 0.4f)
	{
		// 총알 삭제
		Set_Dead(); //투사체는 사라짐 
		m_tBullet.fDeleteTime = 0.f;
	}

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

		MeshSphereColider(pShape->fRadius, 30, 30);
		m_pBufferComp->Render_Buffer();
		
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CPlayerBullet* CPlayerBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vDir, _float MoveSpeed, CAceUnit* _Owner, PLAYER_ATTACK_STATE _AttackState, ETEAM_ID _eTeamID)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("PlayerBullet Create Failed");
		return nullptr;
	}

	// 플레이어 위치에서 생성
	pInstance->m_pTransformComp->Set_Pos(vPos.x, vPos.y, vPos.z);	// 생성 위치
	pInstance->m_tBullet.fMoveSpeed = MoveSpeed;					// 투사체 속도 설정
	pInstance->Set_Owner(_Owner);									// 공격의 주인
	pInstance->Set_Player_AttackState(_AttackState);				// 공격의 상태(공격 유형)
	pInstance->Set_TeamID(_eTeamID);								// 공격의 팀 설정
	pInstance->Set_AttackDir(vDir);

	return pInstance;
}

HRESULT CPlayerBullet::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);


	// -------------------- 충돌 세트 --------------------------
	// 콜라이더 컴포넌트
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);
	
	// 물리 세계 등록
	m_pColliderComp->EnterToPhysics(0);

	// 충돌 함수 연결
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// 충돌 레이어, 마스크 설정
	m_pColliderComp->Set_CollisionLayer(LAYER_PLAYER_ATTACK); // 이 클래스가 속할 충돌레이어 (플레이어 공격)
	m_pColliderComp->Set_CollisionMask(LAYER_MONSTER | LAYER_BOSSMONSTER); // 얘랑 충돌해야하는 레이어들 (몬스터)
	return S_OK;
}

void CPlayerBullet::Free()
{
	SUPER::Free();
}

void CPlayerBullet::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
	
}

void CPlayerBullet::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
	CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

	if (pAceObj == nullptr)
		return;

	if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE)
	{
		// 몬스터 피해  (데미지, 이 공격을 받은 타겟, 이 공격의 유형)
		Change_MonsterHp(-m_tBullet.fDamage, pDst, m_ePlayer_AttackState);

		// Test 공격 확인
		if (m_ePlayer_AttackState == PSITDONW_ATTACK)
		{
			OutputDebugString(L"플레이어가 앉아서 공격함 \n");
		}
		else
		{
			OutputDebugString(L"플레이어의 총알 충돌 \n");
		}

		// 총알 삭제
		Set_Dead(); //투사체는 사라짐 
	}
}

void CPlayerBullet::OnCollisionExited(CGameObject* pDst)
{
}
