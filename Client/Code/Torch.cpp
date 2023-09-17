#include "stdafx.h"
#include "Torch.h"


CTorch::CTorch(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CTorch::CTorch(const CTorch& rhs)
	: Base(rhs)
{
}

CTorch::~CTorch()
{
}

HRESULT CTorch::Ready_GameObject()
{
	return E_NOTIMPL;
}

_int CTorch::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	if (Engine::Get_DIKeyState(DIK_V))
		bActive = !bActive;

	return 0;
}

void CTorch::Render_GameObject()
{
}

HRESULT CTorch::Add_Component()
{
	return E_NOTIMPL;
}

CTorch* CTorch::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return nullptr;
}

void CTorch::Free()
{
}
