#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CBullet : public CGameObject
{
	DERIVED_CLASS(CGameObject, CBullet)

private:
	explicit CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBullet();

protected:
	virtual void	Free();

public:
	virtual HRESULT	Ready_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject() {}

public:

protected:

private:

};

END