#include "stdafx.h"
#include "UI.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{

}

CUI::CUI(const CUI& rhs)
	: Base(rhs)
{

}

CUI::~CUI()
{

}

void CUI::Free()
{
	SUPER::Free();
}

HRESULT CUI::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	return S_OK;
}

_int CUI::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	return 0;
}

void CUI::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

