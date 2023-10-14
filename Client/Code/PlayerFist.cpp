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

	// 충돌
	m_pTransformComp->Readjust_Transform();
	FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = 10.f;

	// 플레이어가 바라보는 쪽으로 날아가게함
	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	// 발사 방향 설정
	m_tFist.vDir = m_pPlayerTransformcomp->Get_Look();

	// 플레이어 받아오기
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	m_bDbugFrame = pPlayer->Get_DBugFrame();

	// 데미지 설정
	m_tFist.fDamage = 5.f;

	return S_OK;
}

_int CPlayerFist::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	//m_pTransformComp->Move_Pos(&m_tFist.vDir, fTimeDelta, m_tFist.fMoveSpeed);

	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 

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
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

		m_pBufferComp->Render_Buffer();

		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CPlayerFist* CPlayerFist::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, CAceUnit* _Owner, PLAYER_ATTACK_STATE _AttackState, ETEAM_ID _eTeamID)
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
	pInstance->Set_Owner(_Owner);									// 공격의 주인
	pInstance->Set_Player_AttackState(_AttackState);			 	// 공격의 상태(공격 유형)
	pInstance->Set_TeamID(_eTeamID);								// 공격의 팀 설정

	return pInstance;
}

HRESULT CPlayerFist::Add_Component()
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
	m_pColliderComp->Set_CollisionMask(LAYER_MONSTER); // 얘랑 충돌해야하는 레이어들 (몬스터)
	m_pColliderComp->Get_CollisionMask();
	return S_OK;
}

void CPlayerFist::Free()
{
	SUPER::Free();
}

void CPlayerFist::OnCollision(CGameObject* pDst)
{


}

void CPlayerFist::OnCollisionEntered(CGameObject* pDst)
{
	OutputDebugString(L"플레이어의 주먹 충돌 \n");

	// 몬스터 피해  (데미지, 이 공격을 받은 타겟, 이 공격의 유형)
	Change_MonsterHp(-m_tFist.fDamage, pDst, m_ePlayer_AttackState);
	
	// Test 앉기 공격 확인
	if (m_ePlayer_AttackState == PSITDONW_ATTACK)
	{
		OutputDebugString(L"플레이어가 앉아서 공격함 \n");
	}

	// 총알 삭제
	Set_Dead(); //투사체는 사라짐 
}

void CPlayerFist::OnCollisionExited(CGameObject* pDst)
{
}
