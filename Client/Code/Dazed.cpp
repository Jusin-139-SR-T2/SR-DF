#include "stdafx.h"
#include "Dazed.h"

CDazed::CDazed(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CDazed::CDazed(const CDazed& rhs)
	: CGameObject(rhs)
{
}

CDazed::~CDazed()
{
}

HRESULT CDazed::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformComp->Set_Pos(5.f, 2.f, 15.f);
	m_pTransformComp->Set_Scale({ 10.f, 1.f, 1.f });

	m_fFrame = 0;
	m_fFrameSpeed = 15.f;	


	m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

	//m_pTransformComp->Set_Pos(m_pBossTransformcomp->Get_Pos());
	return S_OK;
}

_int CDazed::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame > m_fFrameEnd)
	{
		m_fFrame = 0.f;
	}

	Billboard();

	Engine::Add_RenderGroup(RENDER_ALPHATEST , this);

	return S_OK;
}

void CDazed::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CDazed::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CDazed* CDazed::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("LightBeam Create Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CDazed::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_Effect_BeamTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	return S_OK;
}

void CDazed::Free()
{
	SUPER::Free();
}

void CDazed::Billboard()
{
	_matrix		matWorld, matView, matBill;

	matWorld = *m_pTransformComp->Get_Transform();

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);

	m_pTransformComp->Set_WorldMatrixS(&(matBill * matWorld));
}
