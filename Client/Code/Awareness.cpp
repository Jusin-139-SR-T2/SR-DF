#include "stdafx.h"
#include "Awareness.h"

CAwareness::CAwareness(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CAwareness::CAwareness(const CAwareness& rhs)
	: Base(rhs)
{
}

CAwareness::~CAwareness()
{
}

CAwareness* CAwareness::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CAwareness::TYPE pType, CGameObject* powner)
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
	pInstance->Set_Owner(powner);

	return pInstance;
}

HRESULT CAwareness::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformComp->Set_Scale({ 1.f, 1.f, 1.f });

	m_fFrame = 0;

	m_fAge = 0.f;
	m_fLifeTime = 1.f;
	m_fAwareness = 0.f;

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Awareness");
	m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

	m_fBossAwareness.Cur = 0.f;
	m_fBossAwareness.Max = 15.f;
	m_fBossAwarenessPrevCur = 0.f;

	return S_OK;
}

_int CAwareness::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Billboard_Aware();

	Update_BlackBoard(); //보스의 경우 블랙보드로 가져와서 쓸 수 있다

	if (m_bBossConnect) // 보스 Awareness 연결되었을경우 쓰는곳 - Frame안씀 
	{
		if (m_fBossAwarenessPrevCur > m_fBossAwareness.Cur) // 감소할때 
		{
			m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"MissTarget");
			
			if(m_fBossAwareness.Cur<0.1)
				m_bTrigger = TRUE;
		}
		if (m_fBossAwarenessPrevCur < m_fBossAwareness.Cur) // 증가할때 
			m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Awareness");

		
	}
	else //BROWN + GRAY - 변경해야함 
	{
		m_fFrame += fTimeDelta * m_fFrameSpeed;

		if (m_fFrame > m_fFrameEnd)
		{
			m_fFrame = m_fFrameEnd - 1;
			m_bTrigger = TRUE;
		}

		
	}

	if (m_bTrigger) //없애는 트리거 
	{
		m_fAge += fTimeDelta * 1.f;

		if (m_fAge > m_fLifeTime)
		{
			Set_Dead();
		}
	}
	// 테스트 빌드

	/*if (Engine::IsKey_Pressed(DIK_P))
	{
		swprintf_s(debugString, L"변수 확인 Awareness = %f\n", m_fBrownAwareness);
		OutputDebugStringW(debugString);
	}
	*/

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

	if(m_bBossConnect)
		m_pTextureComp->Render_Texture(m_fBossAwareness.Cur);
	else
		m_pTextureComp->Render_Texture(_ulong(m_fFrame));

	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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
	m_fBossAwarenessPrevCur = m_fBossAwareness.Cur;  // 이전정보 저장
	m_fBossAwareness.Cur = pBlackBoard->Get_BossAwareness().Cur; // 새로운 정보 불러오기 

	if (m_fBossAwareness.Cur >= 0.1)
		m_bBossConnect = TRUE;
}
