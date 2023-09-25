#include "stdafx.h"
#include "UI.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "DynamicCamera.h"
#include "CalculatorComponent.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev): CGameObject(pGraphicDev), fHp(100.f)
{
}

CUI::CUI(const CUI& rhs): CGameObject(rhs)
{
}

CUI::~CUI()
{
}

HRESULT CUI::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	/* 직교투영행렬을 만든다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.0f, 100.0f);

	//m_pTransformComp->m_vScale.x = 1.5f;
	//m_pTransformComp->m_vScale.y = 1.5f;

	//사이즈 : 200
	m_fSizeXT = 100;
	m_fSizeY = 300;

	//위치 : 100 = 200 / 2
	m_fX = m_fSizeXT * 0.f;
	m_fY = m_fSizeY * 0.5f;

	D3DXMatrixIdentity(&m_ViewMatrix);

	//				  사이즈 100 =	200 / 2
	m_pTransformComp->m_vScale.x = m_fSizeXT * 0.5f;
	m_pTransformComp->m_vScale.y = m_fSizeY * 0.5f;

	// 위치 잡아주는 계산		위치 -300 = 100	 - 400
	m_pTransformComp->m_vInfo[INFO_POS].x = 0;
	m_pTransformComp->m_vInfo[INFO_POS].y = -m_fY + WINCY * 0.5f;

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
	return S_OK; 
}

_int CUI::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = SUPER::Update_GameObject(fTimeDelta);

	Key_Input(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUI::LateUpdate_GameObject()
{
	//_vec3 vPos;
	//m_pTransformComp->Get_Info(INFO_POS, &vPos);
	//__super::Compute_ViewZ(&vPos);
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

	// 직교
	_matrix matTest;
	m_pGraphicDev->GetTransform(D3DTS_WORLD, &matTest);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matTest);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matTest);
	// 직교
	float test = m_pTransformComp->m_vInfo[INFO_POS].x;

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
	NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

	return S_OK;
}

void CUI::Key_Input(const _float& fTimeDelta)
{
	//추후에 1,2,3번으로 인벤토리 바꾸는 것


	if (Engine::Get_DIKeyState(DIK_Z) & 0x80)
	{
		fHp -= 1.f;
		m_pTransformComp->Rotation(ROT_X, 0.1f);
	}
	if (Engine::Get_DIKeyState(DIK_X) & 0x80)
	{
		fHp += 1.f;
		m_pTransformComp->Rotation(ROT_X, -0.1f);
	}
}
