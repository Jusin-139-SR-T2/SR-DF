#include "stdafx.h"
#include "PunchGuy.h"

CPunchGuy::CPunchGuy(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CPunchGuy::CPunchGuy(const CMonster& rhs)
	: Base(rhs)
{
}

CPunchGuy::~CPunchGuy()
{
	
}

HRESULT CPunchGuy::Ready_GameObject()
{
	return E_NOTIMPL;
}

_int CPunchGuy::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CPunchGuy::LateUpdate_GameObject()
{
}

void CPunchGuy::Render_GameObject()
{

}

HRESULT CPunchGuy::Add_Component()
{
	return E_NOTIMPL;
}

void CPunchGuy::Free()
{
}
