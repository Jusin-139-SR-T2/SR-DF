#include "stdafx.h"
#include "BlueBuff.h"

CBlueBuff::CBlueBuff(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CBlueBuff::CBlueBuff(const CBlueBuff& rhs)
	: Base(rhs)
{
}

CBlueBuff::~CBlueBuff()
{
}

CBlueBuff* CBlueBuff::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, MonsterPhase _CurrPhase)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("LightBeam Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Value_Setting(_x, _y, _z, _CurrPhase);

	return pInstance;
}

HRESULT CBlueBuff::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// 이미지 
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Buff");

	// 프레임 및 사망시간 조정
	m_fFrame = 0;
	m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_fFrameSpeed = 8.f;

	// 크기조정
	m_pTransformComp->Set_Scale({ 2.f, 2.f, 1.f });

	return S_OK;
}

_int CBlueBuff::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	CTransformComponent* m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Boss", L"Com_Transform"));
	m_pTransformComp->Set_Pos(m_pPlayerTransformcomp->Get_Pos());

	m_fFrame += fTimeDelta * m_fFrameSpeed;

	if (m_fFrame > m_fFrameEnd)
	{
		m_fFrame = 0;
	}

	if (m_bBossDead)
		Set_Dead();

	Billboard();

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

#pragma region 기본셋팅 

void CBlueBuff::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CBlueBuff::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBlueBuff::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_Effect_BeamTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	return S_OK;
}

void CBlueBuff::Free()
{
	SUPER::Free();
}

HRESULT CBlueBuff::Billboard()
{
	// 몬스터가 플레이어 바라보는 벡터 
	CTransformComponent* m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	_vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

	D3DXVec3Normalize(&vDir, &vDir);

	_float rad = atan2f(vDir.x, vDir.z);

	m_pTransformComp->Set_RotationY(rad);

	return S_OK;
}

#pragma endregion

void CBlueBuff::Value_Setting(_float _x, _float _y, _float _z, MonsterPhase _phase)
{
	//값 넣어주기 

}
