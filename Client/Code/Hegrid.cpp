#include "stdafx.h"
#include "Hegrid.h"
#include "Player.h"

void CHegrid::Free()
{
	SUPER::Free();
}

CHegrid::CHegrid(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CHegrid::CHegrid(const CHegrid& rhs)
	: Base(rhs)
{
}

CHegrid::~CHegrid()
{
}

HRESULT CHegrid::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	//Add_Component();
	// 이미지 및 프레임 셋팅
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Hegrid", L"Idle");
	m_pTextureComp->Readjust_Transform();
	//m_pTextureComp->Set_Scale({ 1.f, 1.f, 1.f });

	_vec3 vScale = { 5.f, 5.f, 5.f };
	m_pTransformComp->Set_Scale(vScale);

	m_tFrame.fFrame = 0.f;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 0.f;
	m_tFrame.fRepeat = 0.f;

	return S_OK;
}

HRESULT CHegrid::Ready_GameObject(const FSerialize_GameObject tObjectSerial)
{

	return S_OK;
}

_int CHegrid::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	//Get_PlayerPos();

	Billboard(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CHegrid::LateUpdate_GameObject()
{
}

void CHegrid::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &((*m_pTextureComp->Get_Transform()) * (*m_pTransformComp->Get_Transform())));

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CHegrid* CHegrid::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Hegrid Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z); // 시작위치 설정

	return pInstance;
}

HRESULT CHegrid::Add_Component()
{
	// 버퍼 컴포넌트
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);

	return S_OK;
}

HRESULT CHegrid::Billboard(const _float& fTimeDelta)
{
	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	// 플레이어를 바라보게 함
	_vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

	D3DXVec3Normalize(&vDir, &vDir);

	_float rad = atan2f(vDir.x, vDir.z);

	m_pTransformComp->Set_RotationY(rad);

	return S_OK;
}

//HRESULT CHegrid::Get_PlayerPos()
//{
//	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
//	m_vPos = m_pTransformComp->Get_Pos();
//
//	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
//	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);
//
//
//	return S_OK;
//}