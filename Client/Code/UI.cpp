#include "stdafx.h"
#include "UI.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "DynamicCamera.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev): CGameObject(pGraphicDev), fHp(100.f)
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject(rhs)
{
}

CUI::~CUI()
{
}

HRESULT CUI::Ready_GameObject()
{
	D3DXMatrixIdentity(&m_ViewMatrix);

	/* 직교투영행렬을 만든다. */

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fSizeX = 128;
	m_fSizeY = 128;

	m_fX = m_fSizeX * 0.5f; // 중점위치 
	m_fY = m_fSizeY * 0.5f;

	m_pTransformComp->m_vScale.x = m_fSizeX * 0.5f; // 이미지 크기 
	m_pTransformComp->m_vScale.y = m_fSizeY * 0.5f;
				   
	m_pTransformComp->m_vInfo[INFO_POS].x = m_fX - WINCX * 0.5f;
	m_pTransformComp->m_vInfo[INFO_POS].y = -m_fY + WINCY * 0.5f;

	D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.0f, 100.0f);
	return S_OK;
}

_int CUI::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = SUPER::Update_GameObject(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUI::LateUpdate_GameObject()
{
}

CUI* CUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject())) {
		Safe_Release(pInstance);

		MSG_BOX("UI Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureComp->Render_Texture(0);
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CUI::Free()
{
	SUPER::Free();
}

HRESULT CUI::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_UITextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	
	return S_OK;
}

void CUI::Key_Input(const _float& fTimeDelta)
{
	//추후에 1,2,3번으로 인벤토리 바꾸는 것
	if (Engine::Get_DIKeyState(DIK_Z) & 0x80)
	{
		fHp -= 1.f;
	}
	if (Engine::Get_DIKeyState(DIK_X) & 0x80)
	{
		fHp += 1.f;
	}
}
