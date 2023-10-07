#include "stdafx.h"
#include "ThrowPipe.h"

CThrowPipe::CThrowPipe(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CThrowPipe::CThrowPipe(const CThrowPipe& rhs)
	: CGameObject(rhs)
{
}

CThrowPipe::~CThrowPipe()
{
}

HRESULT CThrowPipe::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformComp->Set_Scale({ 0.3f, 0.7f, 1.f });

	m_fFrame = 0.f;
	m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_fFrameSpeed = 10.f;

	// 플레이어 쪽으로 날아가기위해 방향벡터 잡기 
	


	return S_OK;
}

_int CThrowPipe::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	//m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	//NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	//m_vDir = m_pTransformComp->Get_Pos() - m_pPlayerTransformcomp->Get_Pos();
	//m_fMoveSpeed = 3.f;
	//D3DXVec3Normalize(&m_vDir, &m_vDir); //정규화 해서 스피드로 제어

	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame > m_fFrameEnd)
	{
		m_fFrame = 0.f;
	}

	//m_pTransformComp->Move_Pos(&m_vDir, fTimeDelta, m_fMoveSpeed);

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CThrowPipe::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CThrowPipe::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CThrowPipe* CThrowPipe::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
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

HRESULT CThrowPipe::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_ProjectileTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	
	//// 콜라이더 컴포넌트
	//NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);
	//// 물리 세계 등록
	//m_pColliderComp->EnterToPhysics(0);
	//// 충돌 함수 연결
	//m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	//m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	//m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	return S_OK;
}

void CThrowPipe::Free()
{
	SUPER::Free();
}

//void CThrowPipe::OnCollisionEntered(CGameObject* pDst)
//{
//	OutputDebugString(L"Gray의 투사체와 충돌시작 \n");
//
//	Set_Dead(); //투사체는 사라짐 
//}
//
//void CThrowPipe::OnCollisionExited(CGameObject* pDst)
//{
//}
