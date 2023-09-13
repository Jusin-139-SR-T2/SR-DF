#include "Bullet.h"

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CBullet::~CBullet()
{
}

void CBullet::Free()
{
}

HRESULT CBullet::Ready_GameObject()
{
	return E_NOTIMPL;
}

_int CBullet::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}
