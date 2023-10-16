#include "Awareness.h"

CAwareness::CAwareness(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CAwareness::CAwareness(const CAwareness& rhs)
	:Base(rhs)
{
}

CAwareness::~CAwareness()
{
}

CAwareness* CAwareness::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CAwareness::TYPE pType, CGameObject* pOwner)
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
	pInstance->m_eType = pType;

	return pInstance;
}

HRESULT CAwareness::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Awareness");
	m_pTransformComp->Set_Scale({ 1.f, 1.f, 1.f });

	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrame = 0;

	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 1.f;

	m_fAwareness = 0.f;
	m_fMaxAwareness = 15.f;

	return S_OK;
}

_int CAwareness::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Billboard_Aware();	
	
	m_fAwarenessPrev = m_fAwareness;

	Update_Awareness();

	if(m_fAwarenessPrev > m_fAwareness) // 감소할때 
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"MissTarget");

	if (m_fAwarenessPrev < m_fAwareness) // 증가할때 
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Awareness");

	if (m_fAwareness >= m_fMaxAwareness)
	{
		m_fAwareness = m_fMaxAwareness - 1;
		m_bTrigger = TRUE;
	}
	
	if (m_bTrigger) // 변수 다 차면 이미지 바꾸고 1초뒤 삭제용 
	{
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"MaxAware");

		m_tFrame.fAge += fTimeDelta * 1.f;

		if (m_tFrame.fAge > m_tFrame.fLifeTime)
		{
			Set_Dead();
		}
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

	if (m_bTrigger)
		m_pTextureComp->Render_Texture();
	else
		m_pTextureComp->Render_Texture(_ulong(m_fAwareness));

	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CAwareness::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	return S_OK;
}

HRESULT CAwareness::Billboard_Aware()
{
	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	_vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

	D3DXVec3Normalize(&vDir, &vDir);

	_float rad = atan2f(vDir.x, vDir.z);

	m_pTransformComp->Set_RotationY(rad - D3DX_PI);

	return S_OK;
}

HRESULT CAwareness::Update_Awareness()
{
	// 또 오류터지면 static으로 걍 고정

	if (CAwareness::TYPE::BROWN == m_eType)
	{
		CBrown* pBrown = dynamic_cast<CBrown*>(m_pOwner);
		m_fAwareness = pBrown->Get_Awareness();
	}
	else if (CAwareness::TYPE::GRAY == m_eType)
	{
		CGray* pGray = dynamic_cast<CGray*>(m_pOwner);
		m_fAwareness = pGray->Get_Awareness();
	}
	else if (CAwareness::TYPE::BOSS == m_eType)
	{
		CAceBoss* pBoss = dynamic_cast<CAceBoss*>(m_pOwner);
		m_fAwareness = pBoss->Get_Awareness();
	}

	return S_OK;
}

void CAwareness::Update_BlackBoard()
{
	if (!m_wpBlackBoard_Monster.Get_BlackBoard())
	{
		m_wpBlackBoard_Monster.Set_BlackBoard(Engine::Get_BlackBoard(L"MonsterUnion"));
		// 연결 실패
		if (!m_wpBlackBoard_Monster.Get_BlackBoard())
			return;
	}

	CBlackBoard_Monster* pBlackBoard = m_wpBlackBoard_Monster.Get_BlackBoard();

	// 여기서부터 블랙보드의 정보를 얻어온다.
	//m_fBossAwarenessPrevCur = m_fBossAwareness.Cur;  // 이전정보 저장
	//m_fAwareness = pBlackBoard->Get_BossAwareness().Cur; // 새로운 정보 불러오기 
}

void CAwareness::Free()
{
	SUPER::Free();
}