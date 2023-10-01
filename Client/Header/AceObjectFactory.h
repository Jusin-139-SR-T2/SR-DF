#pragma once
#include "GameObject.h"


class CAceObjectFactory : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CAceObjectFactory)

public:
enum class OBJECT_CLASS { 
	FOOD,  //¿Ï
	WEAPON, //¿Ï 
	DISPLAY,  
	INTERACTION, //¿Ï 
	THROW, 

	OBJECT_CLASSEND };

public:
	static Engine::CGameObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, OBJECT_CLASS objClass, const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz);
};
