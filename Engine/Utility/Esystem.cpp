#include "Esystem.h"

CEsystem::CEsystem(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CEsystem::CEsystem(const CEsystem& rhs)
	: CGameObject(rhs)
{
}

CEsystem::~CEsystem()
{
}

HRESULT CEsystem::Ready_GameObject(const _tchar* _stateKey)
{
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", _stateKey);

	return E_NOTIMPL;
}

_int CEsystem::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CEsystem::LateUpdate_GameObject()
{
}

void CEsystem::Render_GameObject()
{
}

HRESULT CEsystem::Add_Component()
{
	return E_NOTIMPL;
}

void CEsystem::Billboard()
{
}

void CEsystem::Free()
{
	SUPER::Free();
}