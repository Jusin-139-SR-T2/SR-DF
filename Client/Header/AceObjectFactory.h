#pragma once
#include "GameObject.h"


class CAceObjectFactory : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CAceObjectFactory)

public:
enum class OBJECT_CLASS { FOOD, WEAPON, DISPLAY, INTERACTION, KEY, THROW_WEAPON, OBJECT_CLASSEND };


	

private:
	explicit CAceObjectFactory(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceObjectFactory(const CAceObjectFactory& rhs);
	virtual ~CAceObjectFactory();

public:
	static Engine::CGameObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, OBJECT_CLASS objClass, const _tchar* pObjTag);
};
