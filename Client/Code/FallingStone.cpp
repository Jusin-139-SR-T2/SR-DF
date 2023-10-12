#include "stdafx.h"
#include "FallingStone.h"

CFallingStone::CFallingStone(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CFallingStone::CFallingStone(const CFallingStone& rhs)
	: Base(rhs)
{
}

CFallingStone::~CFallingStone()
{
}

CFallingStone* CFallingStone::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, MonsterPhase _pHASE, CAceUnit* pOwner)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("LightBeam Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);

	return pInstance;
}

HRESULT CFallingStone::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	srand((_uint)time(NULL));

	// 충돌용
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
	pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = 0.5f;

	// 떨어지는 돌은 랜덤 이미지 
	int iCombo = (rand() % 2) + 1;
	if (1 == iCombo)
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Stone1");
	else
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Stone2");

	// 프레임 및 사망시간 조정
	m_tFrame.fFrame = 0;
	m_tFrame.fFrameSpeed = 3.f;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	
	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 1.f;
	
	m_fFallingSpeed = 6.f;

	// 크기조정
	m_pTransformComp->Set_Scale({ 2.f, 2.f, 1.f });

	// 기타 옵션값
	vFall = { 0.f, -1.f, 0.f };
	return S_OK;
}

_int CFallingStone::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Billboard();
	
	if(m_bFall)
		Falling(fTimeDelta); //낙하 

	// 안맞고 그냥 낙하해서 땅에 충돌 or 무언가에 충돌 
	if (m_bCollision)
	{
		m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tFrame.fFrame = m_tFrame.fFrameEnd - 1;
			Set_Dead();
		}
	}


	//물리 업데이트 코드
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 콜라이더 위치 업데이트 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CFallingStone::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CFallingStone::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

#pragma region 기본셋팅 

HRESULT CFallingStone::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_Effect_BeamTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	
	// 콜라이더 컴포넌트
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);

	// 물리 세계 등록
	m_pColliderComp->EnterToPhysics(0);

	// 충돌 함수 연결
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// 충돌 레이어, 마스크 설정
	m_pColliderComp->Set_CollisionLayer(LAYER_PROJECTILE); // 이 클래스가 속할 충돌레이어 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER); // 얘랑 충돌해야하는 레이어들 

	return S_OK;
}

void CFallingStone::Free()
{
	SUPER::Free();
}

void CFallingStone::Falling(const _float& fTimeDelta)
{
	if (m_pTransformComp->Get_Pos().y < 0.5f)
	{
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"FallingStone");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_bCollision = TRUE;
	}
	else
		m_pTransformComp->Move_Pos(&vFall, fTimeDelta, m_fFallingSpeed);

}
#pragma endregion

void CFallingStone::OnCollision(CGameObject* pDst)
{
}

void CFallingStone::OnCollisionEntered(CGameObject* pDst)
{
	OutputDebugString(L"▶FallingStone 충돌 \n");

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"FallingStone");
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_bCollision = TRUE; // 충돌한뒤 이미지 바꾸기용도 
	m_bFall = FALSE; //충돌하면 더이상 떨어지지 말아야하므로 

	Change_PlayerHp(-2.f);
}

void CFallingStone::OnCollisionExited(CGameObject* pDst)
{
}
