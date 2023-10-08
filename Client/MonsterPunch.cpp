
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

	// 충돌
	m_pTransformComp->Readjust_Transform();
	FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = 1.5f;

	//디버그용 텍스쳐
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Debug", L"Sphere");

	return S_OK;
}

_int CMonsterPunch::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 

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

	// 생성할때 몬스터 위치 로 생성하기 위해 Create에서 초기위치를 잡아줌 
	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);

	return pInstance;
}

HRESULT CMonsterPunch::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_ProjectileTextureComp"), E_FAIL);
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
	m_pColliderComp->Set_CollisionLayer(ELAYER_PROJECTILE); // 이 클래스가 속할 충돌레이어 
	m_pColliderComp->Set_CollisionMask(ELAYER_PLAYER); // 얘랑 충돌해야하는 레이어들 

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
	OutputDebugString(L"▶▶▶ Debug Attack과 충돌시작 \n");

	Set_Dead(); //투사체는 사라짐 
}

void CMonsterPunch::OnCollisionExited(CGameObject* pDst)
{
}
