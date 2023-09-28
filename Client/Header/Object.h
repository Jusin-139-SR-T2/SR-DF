#pragma once
#include "Base.h"
#include "GameObject.h"

class CObject :
    public CGameObject
{
private:
	explicit CObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CObject(const CObject& rhs);
	virtual ~CObject();


};

