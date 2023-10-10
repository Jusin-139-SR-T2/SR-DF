#include "stdafx.h"
#include "Awareness.h"

CAwareness::CAwareness(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
	, m_fBrownAwareness(0), m_fGrayAwareness(0), m_fBossAwareness(0)
{
}

CAwareness::CAwareness(const CAwareness& rhs)
	: CGameObject(rhs)
{
}

CAwareness::~CAwareness()
{
}

HRESULT CAwareness::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformComp->Set_Scale({ 1.f, 1.f, 1.f });

	m_fFrame = 0;

	m_fAge = 0.f;
	m_fLifeTime = 1.f;

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Awareness");
	m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

	
	return S_OK;
}

_int CAwareness::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	// 몬스터 상태 Taunt로 바뀌면 바로 set_dead로 가기 
	Billboard();

	//Owner 지정으로 알아보기

	m_fFrame += fTimeDelta * m_fFrameSpeed;

	if (m_fFrame > m_fFrameEnd)
	{
		m_fFrame = m_fFrameEnd - 1;
		m_bTrigger = TRUE;
	}

	if (m_bTrigger)
		m_fAge += fTimeDelta * 1.f;
	

	if (m_fAge > m_fLifeTime)
	{
		m_bTrigger = FALSE;
		Set_Dead();
	}
	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CAwareness::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CAwareness::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CAwareness* CAwareness::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CAwareness::TYPE pType)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("LightBeam Create Failed");
		return nullptr;
	}

	// 생성할때 몬스터 위치 로 생성하기 위해 Create에서 초기위치를 잡아줌 
	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->m_eType = pType;
	pInstance->Set_Speed(pType);

	return pInstance;
}

HRESULT CAwareness::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_Effect_BeamTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	return S_OK;
}

void CAwareness::Free()
{
	SUPER::Free();
}

HRESULT CAwareness::Billboard()
{
	CTransformComponent* m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	// 이펙트가 플레이어 보는 벡터 
	_vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();  

	D3DXVec3Normalize(&vDir, &vDir);

	_float rad = atan2f(vDir.x, vDir.z);

	m_pTransformComp->Set_RotationY(rad - D3DX_PI);

	return S_OK;
}

void CAwareness::Set_Speed(CAwareness::TYPE pType)
{
	switch (pType)
	{
	case CAwareness::TYPE::BROWN:
		m_fFrameSpeed = 2.f;
		break;
	case CAwareness::TYPE::GRAY:
		m_fFrameSpeed = 3.f;
		break;
	case CAwareness::TYPE::BOSS:
		m_fFrameSpeed = 4.f;
		break;
	}
}
