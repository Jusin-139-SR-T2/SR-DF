#include "stdafx.h"
#include "UI_Player.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "DynamicCamera.h"

CUI_Player::CUI_Player(LPDIRECT3DDEVICE9 pGraphicDev) 
	: Base(pGraphicDev), m_fHp(100.f)
{
}

CUI_Player::CUI_Player(const CUI_Player& rhs)
	: Base(rhs)
{
}

CUI_Player::~CUI_Player()
{
}

HRESULT CUI_Player::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fSizeX = 200;
	m_fSizeY = 140;

	m_fX = m_fSizeX * 0.5f; // 중점위치 
	m_fY = m_fSizeY * 0.5f + WINCY - m_fSizeY;

	m_pTextureComp->Set_Pos({ m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f });	// 이미지 위치
	m_pTextureComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });							// 이미지 크기

	return S_OK;
}

_int CUI_Player::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = SUPER::Update_GameObject(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUI_Player::LateUpdate_GameObject()
{
}

CUI_Player* CUI_Player::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject())) {
		Safe_Release(pInstance);

		MSG_BOX("UI Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CUI_Player::Render_GameObject()
{
	m_pTextureComp->Readjust_Transform();
	m_pTextureComp->Render_Texture(0, true);
	m_pBufferComp->Render_Buffer();
}

void CUI_Player::Free()
{
	SUPER::Free();
}

HRESULT CUI_Player::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_UITextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	return S_OK;
}

void CUI_Player::Key_Input(const _float& fTimeDelta)
{
	//추후에 1,2,3번으로 인벤토리 바꾸는 것
	if (Engine::Get_DIKeyState(DIK_Z) & 0x80)
	{
		m_fHp -= 1.f;
	}
	if (Engine::Get_DIKeyState(DIK_X) & 0x80)
	{
		m_fHp += 1.f;
	}
}