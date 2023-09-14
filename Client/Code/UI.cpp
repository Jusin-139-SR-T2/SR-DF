#include "stdafx.h"
#include "UI.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "DynamicCamera.h"
#include "Calculator.h"

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

	m_pTransformComp->m_vScale.x = 1.5f;
	m_pTransformComp->m_vScale.y = 1.5f;

	m_pTransformComp->Set_Pos(-9.3f, -4.6f, 10.f);

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
	_vec3 vPos;
	m_pTransformComp->Get_Info(INFO_POS, &vPos);
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
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcTex>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTexture>(ID_STATIC, L"Com_Texture", L"Proto_UITextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransform>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculator>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

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
